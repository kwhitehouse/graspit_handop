//######################################################################
//
// GraspIt!
// Copyright (C) 2002-2009  Columbia University in the City of New York.
// All rights reserved.
//
// This software is protected under a Research and Educational Use
// Only license, (found in the file LICENSE.txt), that you should have
// received with this distribution.
//
// Author(s):  Hao Dang and Matei T. Ciocarlie
//
// $Id: dbaseDlg.h,v 1.14 2009/07/06 20:35:25 cmatei Exp $
//
//######################################################################

/*! \file 
  \brief Defines the %DBaseDlg class
 */

#ifndef _dbasedlg_h_
#define _dbasedlg_h_

#include "ui_dbaseDlg.h"
#include <map>
#include <QDialog>

class GraspitDBGrasp;
class GraspitDBModel;
class Hand;

namespace db_planner {
	class DatabaseManager;
	class Model;
	class Grasp;
}
Hand * setHand(Hand * currentHand, int handInd);
/*! The DBaseDlg serves three purposes: first, it initializes the connection to the dbase
	by creating an SqlDBMgr. Second, it serves a DBase browser, allowing the user to load
	dbase models into the GraspIt world and see grasps on them. Third, it will be used to
	start the DBase planner and pass it the DBMgr.
*/
class DBaseDlg : public QDialog, public Ui::DBaseDlgUI
{
	Q_OBJECT
private:
	//! The last model from the dbase that has been added to the Graspit world
	GraspitDBModel *mCurrentLoadedModel;
	//! Widget for displaying thumbnails
	QGraphicsScene * mModelScene;
	//! The mgr that all connections to the dbase have to go through
	db_planner::DatabaseManager *mDBMgr;
	//! The list of models available in the dbase, as retrieved by the DBMgr
	std::vector<db_planner::Model*> mModelList;
	//! The map from model name to model index in mModelList
	std::map<std::string, int> mModelMap;
	//! A list of grasps for a dbase model, retrieved from the DBMgr
	std::vector<db_planner::Grasp*> mGraspList;
	//! An index for the current grasp shown on the screen
	int mCurrentFrame;
	//! Helper variable to disable the modelChanged trigger
	bool inModelConstruction;

	//! Gets the current manager from the GraspitGUI. If one exists, it also queries for models
	void init();
	//! Also destroys the model scene
	void destroy();
	//! Gets the model list and other data from the database and displays it
	void getModelList();
	//! Populates the drop-down box with names of objects retrieved from DBMgr
	void displayModelList();
	//! Populates the drop-down box with types of grasps in CGDB
	void displayGraspTypeList(std::vector<std::string>);
	//! Helper function that deletes the previously loaded grasps/models
	template <class vectorType, class treatAsType> // ugly but...
	void deleteVectorElements(std::vector<vectorType>&);
	//! Shows the i-th grasp
	void showGrasp(int i);
	//! Shows the next grasp
	void nextGrasp();
	//! Shows the previous grasp
	void previousGrasp();
	//! Updates the related labels in the UI
	void updateGraspInfo();
	void reRunButton_clicked();
	//! Helper function that sets grasp's information texts to default
	void initializeGraspInfo();
	//! show the ith grasp, but back the palm with auto opening
	void gotoGrasp(int i);

public:
	DBaseDlg(QWidget *parent = 0) : QDialog(parent), mCurrentLoadedModel(NULL), mDBMgr(NULL), 
									mModelScene(NULL), mCurrentFrame(0), 
									inModelConstruction(false) {
		setupUi(this);
		QObject::connect(exitButton, SIGNAL(clicked()), this, SLOT(exitButton_clicked()));
		QObject::connect(connectButton, SIGNAL(clicked()), this, SLOT(connectButton_clicked()));
		QObject::connect(loadModelButton, SIGNAL(clicked()), this, SLOT(loadModelButton_clicked()));
		QObject::connect(loadHandButton, SIGNAL(clicked()), this, SLOT(loadHandButton_clicked()));
		QObject::connect(loadGraspButton, SIGNAL(clicked()), this, SLOT(loadGraspButton_clicked()));
		QObject::connect(nextGraspButton, SIGNAL(clicked()), this, SLOT(nextGraspButton_clicked()));
		QObject::connect(previousGraspButton, SIGNAL(clicked()), this, SLOT(previousGraspButton_clicked()));
		QObject::connect(plannerButton, SIGNAL(clicked()), this, SLOT(plannerButton_clicked()));
		QObject::connect(createGWSButton, SIGNAL(clicked()), this, SLOT(createGWSButton_clicked()));
		QObject::connect(utilButton, SIGNAL(clicked()), this, SLOT(utilButton_clicked()));

		QObject::connect(modelsComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(modelChanged()));
		QObject::connect(classesComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(classChanged()));
		QObject::connect(showPreGraspRadioButton, SIGNAL(toggled(bool)), this, SLOT(graspTypeChanged()));
		QObject::connect(showFinalGraspRadioButton, SIGNAL(toggled(bool)), this, SLOT(graspTypeChanged()));

		init();
	}
	~DBaseDlg(){destroy();}

public slots:
	//! Button events
	void connectButton_clicked();
	void exitButton_clicked();
	void loadHandButton_clicked();
	void loadGraspButton_clicked();
	void loadModelButton_clicked();
	void nextGraspButton_clicked();
	void previousGraspButton_clicked();
	void plannerButton_clicked();
	void createGWSButton_clicked();
	void utilButton_clicked();
	//! Trigger events
	void modelChanged();
	void graspTypeChanged();
	void classChanged();
};
#endif
