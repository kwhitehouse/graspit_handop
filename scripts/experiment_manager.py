import eigenhand_db_tools
import generation_manager
import eigenhand_db_objects
import server_list
import time
import remote_dispatcher


class ExperimentManager(object):
    def __init__(self, num_ga_iters, num_atr_iters, task_model_list, task_prototype,
                 trials_per_task, eval_functor, db_interface, start_ga_iter = 0,
                 server_dict = server_list.clic_lab_dict):
        """
        @param num_ga_iters - The number of genetic algorithm generations to run
        @param num_atr_iters - The number of ATR iterations to run for each GA generation
        @param task_model_list - A list of TaskModels used to set up the database planner
        @param task_prototype - A prototype Task object that defines the trial specification(i.e. length and type)
        @param eval_functor - The function to use when evaluating hand fitness in the genetic algorithm
        @param db_interface - The database interface to use. Assumed to be initialized with a starting set of 0th generation hands
        @param starting_ga_iter - The generation to start from.
        @param server_dict - A dictionary whose keys are server urls that will be filled with
                             Servers as connections are made
        """
        self.task_prototype = task_prototype
        self.task_model_list = task_model_list
        self.num_ga_iters = num_ga_iters
        self.num_atr_iters = num_atr_iters
        self.eval_functor = eval_functor
        self.db_interface = db_interface
        self.starting_ga_iter = 0
        self.trials_per_task = trials_per_task
        self.gm = []
        self.server_dict = server_dict
        
    
        
    def get_current_ga_iteration(self):
       
        if not self.gm:
            error("ExperimentManager::get_current_ga::no_generation_manager","Error: Generation manager not initialized!")
            
        return self.gm.generation//self.get_generations_per_ga_iter()

    def get_generations_per_ga_iter(self):
        """
        @brief The generations of the gm counts the number of times a new set of hands is generated. This happens
        num_atr_iters times for each round of ATR and one additional time to generate the new hands for the GA
        totalling num_atr_itrs + 1 per each round of GA
        """
        return self.num_atr_iters + 1
    
    def get_last_ga_generation(self):
        """
        @brief Get the number of the last generation number of hands that were generated by a genetic algorithm, as opposed to ATR
        """
        ga_iter = self.get_current_ga_iteration()
        ga_generation = self.gm.generation//self.get_generations_per_ga_iter()
        return ga_generation
        
    def initialize_generation_manager(self):
        """
        @brief Create the generation manager that will be used in this experiment
        """
        self.gm = generation_manager.GenerationManager(self.db_interface, self.task_model_list, self.starting_ga_iter*self.get_generations_per_ga_iter(),
                                    self.task_prototype.task_time, self.trials_per_task, self.task_prototype.task_type_id,
                                    self.eval_functor)

    

        
    def run_remote_dispatcher_tasks(self):
        """
        @brief Run the tasks on a cluster using the RemoteDispatcher framework developed for this project. 

        """
        #Record the start time
        t = time.time()
        r = 0
        #Test if there are jobs available to do. 
        job_num = eigenhand_db_tools.get_num_unlaunched_jobs(self.db_interface)
        
        #Try to launch a new remote dispatcher as long as there are unfinished jobs
        while job_num > 2:
            print r
            r += 1
            rd = remote_dispatcher.RemoteDispatcher(self.server_dict, self.db_interface)
            #Blocks until all of the remote dispatcher's jobs have terminated, closing all of the connections
            #to remote machines. 
            rd.run_monitored()
            #Reset any incomplete jobs. These are essentially jobs that we have lost connection with and cannot be
            #relied upon to terminate.
            self.db_interface.reset_incompletes()
            #See how many jobs are still undone. 
            job_num = eigenhand_db_tools.get_num_unlaunched_jobs(self.db_interface)
        #If we only have a few stragglers, just stop trying -- We don't really need all of them to exit cleanly,
        #and it is easier to fail gracefully than try to handle every error.
        self.db_interface.set_incompletes_as_error()
        print "done.  Time %f \n"%(time.time() - t)




    def run_experiment(self):
        """
        @brief Run the whole experiment. Does num_ga_iters genetic algorithm runs each containing num_atr iterations
        of ATR.
        """
        #initialize new generation manager to configure the database to start running.    
        self.initialize_generation_manager()
        self.gm.start_generation()

        
        
        for ga_gen_num in range(self.starting_ga_iter, self.num_ga_iters):
            for atr_gen_num in range(self.num_atr_iters):
                #Doing ATR
                #Run the planning jobs
                self.run_remote_dispatcher_tasks()

                #Get the resulting grasps for the latest generation of hands
                grasp_list = gm.get_all_grasps()

                #Run atr on the existing hand for the latest generation of grasps
                new_hand_list = atr.ATR_generation(grasp_list, gm.hands)

                #Update database with new hands
                eigenhand_db_tools.insert_unique_hand_list(new_hand_list)

            # Get the database ready to perform jobs for the next generation
            gm.next_generation()

        #DOING GA
         
        #run the planning jobs
        self.run_remote_dispatcher_tasks()
        #Get the resulting grasps for the latest generation of hands
        grasp_list = self.gm.get_all_grasps()
        
        #Generate new hands based on these grasps, scaling the variance of the mutations down linearly as the
        #generations progress.
        new_hand_list = eigenhand_genetic_algorithm.GA_generation(grasp_list, self.gm.hands, self.eval_functor, .5-.4/self.num_ga_iters*ga_gen_num)
        
        #Put the new hands in to the database.
        eigenhand_db_tools.insert_unique_hand_list(new_hand_list)
        self.gm.next_generation()

        #Plan grasps for the final set of hands.
        self.run_remote_dispatcher_tasks()

        
