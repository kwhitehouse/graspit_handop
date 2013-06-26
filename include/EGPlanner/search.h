#ifndef _search_h_
#define _search_h_

//this defines constants and other things used in searches for good grasps

enum SearchEnergyType{ENERGY_CONTACT, ENERGY_POTENTIAL_QUALITY, ENERGY_CONTACT_QUALITY, 
					  ENERGY_AUTOGRASP_QUALITY, ENERGY_GUIDED_AUTOGRASP, ENERGY_STRICT_AUTOGRASP, ENERGY_CONTACT_AUTOGRASP,
		      ENERGY_COMPLIANT, ENERGY_DYNAMIC, ENERGY_GFO, CONTACT_GFO, NUM_CONTACTS};
enum StateType{SPACE_COMPLETE, SPACE_AXIS_ANGLE, SPACE_ELLIPSOID, SPACE_APPROACH, 
			   POSE_EIGEN, POSE_DOF, ATTRIBUTES};
enum PlannerType{PLANNER_SIM_ANN, PLANNER_MT, PLANNER_LOOP, PLANNER_ONLINE, PLANNER_GT, 
				 PLANNER_TIME_TEST, PLANNER_LIST};

enum RenderType{RENDER_LEGAL, RENDER_BEST, RENDER_NEVER, RENDER_ALWAYS};
enum SearchContactType{CONTACT_PRESET, CONTACT_LIVE};
enum AnnealingType{ANNEAL_DEFAULT, ANNEAL_MODIFIED, ANNEAL_STRICT, ANNEAL_ONLINE, ANNEAL_LOOP};
enum ActionType{ACTION_GRASP, ACTION_OPEN, ACTION_PLAN};

static unsigned char INPUT_NONE=0;
static unsigned char INPUT_GLOVE=1;
static unsigned char INPUT_FILE=2;
//enum InputType{INPUT_NONE = 0, INPUT_FLOCK = 1, INPUT_GLOVE = 2, INPUT_FILE = 4};

#endif
