
/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


//============================================================================
/// \file   MainWindow.cpp
/// \author Uwe Kindler
/// \date   13.02.2018
/// \brief  Implementation of CMainWindow demo class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <MainWindow.h>
#include "ui_mainwindow.h"

#include <iostream>

#include <QTime>
#include <QLabel>
#include <QTextEdit>
#include <QCalendarWidget>
#include <QFrame>
#include <QTreeView>
#include <QFileSystemModel>
#include <QBoxLayout>
#include <QSettings>
#include <QDockWidget>
#include <QDebug>
#include <QResizeEvent>
#include <QAction>
#include <QWidgetAction>
#include <QComboBox>
#include <QInputDialog>

#include <QMap>
#include <QElapsedTimer>

#include "DockManager.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"


//============================================================================
static ads::CDockWidget* createLongTextLabelDockWidget(QMenu* ViewMenu)
{
	static int LabelCount = 0;
	QLabel* l = new QLabel();
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l->setText(QString("Label %1 %2 - Lorem ipsum dolor sit amet, consectetuer adipiscing elit. "
		"Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque "
		"penatibus et magnis dis parturient montes, nascetur ridiculus mus. "
		"Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. "
		"Nulla consequat massa quis enim. Donec pede justo, fringilla vel, "
		"aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, "
		"imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede "
		"mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum "
		"semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, "
		"porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, "
		"dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla "
		"ut metus varius laoreet.")
		.arg(LabelCount)
		.arg(QTime::currentTime().toString("hh:mm:ss:zzz")));

	ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Label %1").arg(LabelCount++));
	DockWidget->setWidget(l);
	DockWidget->setObjectName(DockWidget->windowTitle());
	ViewMenu->addAction(DockWidget->toggleViewAction());
	return DockWidget;
}


//============================================================================
static ads::CDockWidget* createCalendarDockWidget(QMenu* ViewMenu)
{
	static int CalendarCount = 0;
	QCalendarWidget* w = new QCalendarWidget();
	ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Calendar %1").arg(CalendarCount++));
	DockWidget->setWidget(w);
	DockWidget->setObjectName(DockWidget->windowTitle());
	DockWidget->setToggleViewActionMode(ads::CDockWidget::ActionModeShow);
	ViewMenu->addAction(DockWidget->toggleViewAction());
	return DockWidget;
}


//============================================================================
static ads::CDockWidget* createFileSystemTreeDockWidget(QMenu* ViewMenu)
{
	static int FileSystemCount = 0;
	QTreeView* w = new QTreeView();
	w->setFrameShape(QFrame::NoFrame);
	QFileSystemModel* m = new QFileSystemModel(w);
	m->setRootPath(QDir::currentPath());
	w->setModel(m);
	ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Filesystem %1").arg(FileSystemCount++));
	DockWidget->setWidget(w);
	DockWidget->setObjectName(DockWidget->windowTitle());
	ViewMenu->addAction(DockWidget->toggleViewAction());
    return DockWidget;
}


//============================================================================
/**
 * Private data class pimpl
 */
struct MainWindowPrivate
{
	CMainWindow* _this;
	Ui::MainWindow ui;
	QAction* SavePerspectiveAction = nullptr;
	QWidgetAction* PerspectiveListAction = nullptr;
	QComboBox* PerspectiveComboBox = nullptr;;
	ads::CDockManager* DockManager = nullptr;

	MainWindowPrivate(CMainWindow* _public) : _this(_public) {}

	/**
	 * Creates the toolbar actions
	 */
	void createActions();

	/**
	 * Fill the dock manager with dock widgets
	 */
	void createContent();

	/**
	 * Saves the dock manager state and the main window geometry
	 */
	void saveState();

	/**
	 * Save the list of perspectives
	 */
	void savePerspectives();

	/**
	 * Restores the dock manager state
	 */
	void restoreState();

	/**
	 * Restore the perspective listo of the dock manager
	 */
	void restorePerspectives();
};


//============================================================================
void MainWindowPrivate::createContent()
{
	// Test container docking
	QMenu* ViewMenu = ui.menuView;
	auto DockWidget = createCalendarDockWidget(ViewMenu);
	DockWidget->setIcon(_this->style()->standardIcon(QStyle::SP_DialogOpenButton));
	DockWidget->setFeatures(DockWidget->features().setFlag(ads::CDockWidget::DockWidgetClosable, false));
	DockManager->addDockWidget(ads::LeftDockWidgetArea, DockWidget);
	DockManager->addDockWidget(ads::LeftDockWidgetArea, createLongTextLabelDockWidget(ViewMenu));
	DockManager->addDockWidget(ads::BottomDockWidgetArea, createFileSystemTreeDockWidget(ViewMenu));
	auto TopDockArea = DockManager->addDockWidget(ads::TopDockWidgetArea, createFileSystemTreeDockWidget(ViewMenu));
	DockWidget = createCalendarDockWidget(ViewMenu);
	DockWidget->setFeatures(DockWidget->features().setFlag(ads::CDockWidget::DockWidgetClosable, false));
	DockManager->addDockWidget(ads::CenterDockWidgetArea, DockWidget, TopDockArea);

	// Test dock area docking
	auto RighDockArea = DockManager->addDockWidget(ads::RightDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), TopDockArea);
	DockManager->addDockWidget(ads::TopDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), RighDockArea);
	auto BottomDockArea = DockManager->addDockWidget(ads::BottomDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), RighDockArea);
	DockManager->addDockWidget(ads::RightDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), RighDockArea);
	DockManager->addDockWidget(ads::CenterDockWidgetArea, createLongTextLabelDockWidget(ViewMenu), BottomDockArea);
}


//============================================================================
void MainWindowPrivate::createActions()
{
	ui.toolBar->addAction(ui.actionSaveState);
	ui.actionSaveState->setIcon(_this->style()->standardIcon(QStyle::SP_DialogSaveButton));
	ui.toolBar->addAction(ui.actionRestoreState);
	ui.actionRestoreState->setIcon(_this->style()->standardIcon(QStyle::SP_DialogOpenButton));

	SavePerspectiveAction = new QAction("Save Perspective", _this);
	_this->connect(SavePerspectiveAction, SIGNAL(triggered()), SLOT(savePerspective()));
	PerspectiveListAction = new QWidgetAction(_this);
	PerspectiveComboBox = new QComboBox(_this);
	PerspectiveComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	PerspectiveComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	PerspectiveListAction->setDefaultWidget(PerspectiveComboBox);
	ui.toolBar->addSeparator();
	ui.toolBar->addAction(PerspectiveListAction);
	ui.toolBar->addAction(SavePerspectiveAction);
}


//============================================================================
void MainWindowPrivate::saveState()
{
	QSettings Settings("Settings.ini", QSettings::IniFormat);
	Settings.setValue("mainWindow/Geometry", _this->saveGeometry());
	Settings.setValue("mainWindow/State", _this->saveState());
	Settings.setValue("mainWindow/DockingState", DockManager->saveState());
}


//============================================================================
void MainWindowPrivate::restoreState()
{
	QSettings Settings("Settings.ini", QSettings::IniFormat);
	_this->restoreGeometry(Settings.value("mainWindow/Geometry").toByteArray());
	_this->restoreState(Settings.value("mainWindow/State").toByteArray());
	DockManager->restoreState(Settings.value("mainWindow/DockingState").toByteArray());
}



//============================================================================
void MainWindowPrivate::savePerspectives()
{
	QSettings Settings("Settings.ini", QSettings::IniFormat);
	DockManager->savePerspectives(Settings);
}



//============================================================================
void MainWindowPrivate::restorePerspectives()
{
	QSettings Settings("Settings.ini", QSettings::IniFormat);
	DockManager->loadPerspectives(Settings);
	PerspectiveComboBox->clear();
	PerspectiveComboBox->addItems(DockManager->perspectiveNames());
}


//============================================================================
CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	d(new MainWindowPrivate(this))
{
	d->ui.setupUi(this);
	d->createActions();

	// Now create the dock manager and its content
	d->DockManager = new ads::CDockManager(this);
	connect(d->PerspectiveComboBox, SIGNAL(activated(const QString&)),
		d->DockManager, SLOT(openPerspective(const QString&)));

	d->createContent();
	// Default window geometry
	resize(800, 600);

	d->restoreState();
	d->restorePerspectives();
}


//============================================================================
CMainWindow::~CMainWindow()
{
	delete d;
}


//============================================================================
void CMainWindow::closeEvent(QCloseEvent* event)
{
	d->saveState();
	QMainWindow::closeEvent(event);
}


//============================================================================
void CMainWindow::on_actionSaveState_triggered(bool)
{
	qDebug() << "MainWindow::on_actionSaveState_triggered";
	d->saveState();
}


//============================================================================
void CMainWindow::on_actionRestoreState_triggered(bool)
{
	qDebug() << "MainWindow::on_actionRestoreState_triggered";
	d->restoreState();
}


//============================================================================
void CMainWindow::savePerspective()
{
	std::cout << "savePerspective" << std::endl;
	QString PerspectiveName = QInputDialog::getText(this, "Save Perspective", "Enter unique name:");
	if (PerspectiveName.isEmpty())
	{
		return;
	}

	d->DockManager->addPerspective(PerspectiveName);
	QSignalBlocker Blocker(d->PerspectiveComboBox);
	d->PerspectiveComboBox->clear();
	d->PerspectiveComboBox->addItems(d->DockManager->perspectiveNames());
	d->PerspectiveComboBox->setCurrentText(PerspectiveName);

	d->savePerspectives();
}

