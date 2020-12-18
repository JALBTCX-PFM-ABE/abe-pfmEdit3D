
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.
*********************************************************************************************/


/****************************************  IMPORTANT NOTE  **********************************

    Comments in this file that start with / * ! or / / ! are being used by Doxygen to
    document the software.  Dashes in these comment blocks are used to create bullet lists.
    The lack of blank lines after a block of dash preceeded comments means that the next
    block of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

*****************************************  IMPORTANT NOTE  **********************************/



#include "lidarClassDialog.hpp"
#include "lidarClassDialogHelp.hpp"


//!  This is the LiDAR classification dialog.  It is used to select the "from" class(es) and the "to" class for changing LiDAR classification attributes.

lidarClassDialog::lidarClassDialog (QWidget * parent, OPTIONS *op, MISC *mi):
  QDialog (parent, (Qt::WindowFlags) Qt::WA_DeleteOnClose)
{
  options = op;
  misc = mi;

  mod_options = *op;


  setWindowTitle ("pfmEdit3D LiDAR Classification Dialog");


  resize (400, 800);


  setSizeGripEnabled (true);


  setWhatsThis (lidarClassDialogText);


  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  fromGrp = new QButtonGroup (this);
  fromGrp->setExclusive (false);
  connect (fromGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotFrom (int)));

  toGrp = new QButtonGroup (this);
  toGrp->setExclusive (true);


  QHBoxLayout *classBoxLayout[LIDAR_CLASSES];
  QLabel *name[LIDAR_CLASSES];


  for (int32_t i = 0 ; i < LIDAR_CLASSES ; i++)
    {
      classBoxLayout[i] = new QHBoxLayout ();

      fromCheck[i] = new QCheckBox (tr ("From"), this);
      QString tip = tr ("Check this box to allow change from <b>") + misc->lidar_class_name[i] + tr ("</b> to the selected <b>To</b> classification");
      fromCheck[i]->setToolTip (tip);
      fromCheck[i]->setWhatsThis (lidarClassDialogText);
      fromCheck[i]->setChecked (mod_options.from_lidar_class[i]);
      classBoxLayout[i]->addWidget (fromCheck[i]);
      fromGrp->addButton (fromCheck[i], i);

      QString label;

      if (!i)
        {
          label = misc->lidar_class_name[i];
        }
      else
        {
          label = tr ("(%1) ").arg (misc->lidar_class_num[i]) + misc->lidar_class_name[i];
        }
      name[i] = new QLabel (label, this);
      name[i]->setToolTip (misc->lidar_class_desc[i]);
      name[i]->setWhatsThis (lidarClassDialogText);
      classBoxLayout[i]->addWidget (name[i], 10);

      toCheck[i] = new QCheckBox (tr ("To"), this);
      tip = tr ("Check this box to select the <b>") + misc->lidar_class_name[i] + tr ("</b> classification as the target value for the change operation");
      toCheck[i]->setToolTip (tip);
      toCheck[i]->setWhatsThis (lidarClassDialogText);
      if (i == mod_options.lidar_class)
        {
          toCheck[i]->setChecked (true);
        }
      else
        {
          toCheck[i]->setChecked (false);
        }
      classBoxLayout[i]->addWidget (toCheck[i]);
      if (!i) toCheck[i]->hide ();
      toGrp->addButton (toCheck[i], i);

      vbox->addLayout (classBoxLayout[i]);
    }


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);


  QPushButton *bHelp = new QPushButton (this);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *acceptButton = new QPushButton (tr ("Accept"), this);
  acceptButton->setToolTip (tr ("Accept changes made to this dialog and close it"));
  connect (acceptButton, SIGNAL (clicked ()), this, SLOT (slotAccept ()));
  actions->addWidget (acceptButton);

  QPushButton *closeButton = new QPushButton (tr ("Discard"), this);
  closeButton->setToolTip (tr ("Discard changes made to this dialog and close it"));
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotClose ()));
  actions->addWidget (closeButton);


  show ();
}



lidarClassDialog::~lidarClassDialog ()
{
}



void
lidarClassDialog::slotHelp ()
{
  QWhatsThis::enterWhatsThisMode ();
}



void
lidarClassDialog::slotFrom (int id)
{
  disconnect (fromGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotFrom (int)));

  if (id == 0)
    {
      //  If we just checked "Any" we want to uncheck everything else

      if (fromCheck[0]->isChecked ())
        {
          mod_options.from_lidar_class[0] = NVTrue;

          for (int32_t i = 1 ; i < LIDAR_CLASSES ; i++)
            {
              fromCheck[i]->setChecked (false);
              mod_options.from_lidar_class[i] = NVFalse;
            }
        }
    }
  else
    {
      //  If we checked something other than "Any" we need to make sure "Any" is unchecked.

      if (fromCheck[id]->isChecked ())
        {
          mod_options.from_lidar_class[id] = NVTrue;

          fromCheck[0]->setChecked (false);
          mod_options.from_lidar_class[0] = NVFalse;
        }
      else
        {
          mod_options.from_lidar_class[id] = NVFalse;
        }
    }

  connect (fromGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotFrom (int)));
}



void
lidarClassDialog::slotAccept ()
{
  int32_t count = 0;

  for (int32_t i = 0 ; i < LIDAR_CLASSES ; i++)
    {
      if (mod_options.from_lidar_class[i]) count++;
    }

  if (!count)
    {
      QMessageBox::warning (this, tr ("pfmEdit3D LiDAR Classification Dialog"), tr ("You must select at least one <b>From</b> button"));
      return;
    }

  for (int32_t i = 0 ; i < LIDAR_CLASSES ; i++)
    {
      options->from_lidar_class[i] = mod_options.from_lidar_class[i];

      if (toCheck[i]->isChecked ()) options->lidar_class = i;
    }

  emit dataChangedSignal ();

  close ();
}



void
lidarClassDialog::slotClose ()
{
  close ();
}
