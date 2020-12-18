
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



#include "displayMessage.hpp"


displayMessage::displayMessage (QWidget * parent, OPTIONS *op, MISC *mi):
  QDialog (parent, (Qt::WindowFlags) Qt::WA_DeleteOnClose)
{
  options = op;
  misc = mi;


  setWindowTitle (tr ("pfmEdit3D Error Messages"));
  setWhatsThis (tr ("Error and warning messages from sub-programs are displayed here."));


  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);

  messageBox = new QListWidget (this);

  vbox->addWidget (messageBox);


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bClear = new QPushButton (tr ("Clear"), this);
  bClear->setToolTip (tr ("Clear all messages"));
  connect (bClear, SIGNAL (clicked ()), this, SLOT (slotClearMessage ()));
  actions->addWidget (bClear);

  QPushButton *bSaveMessage = new QPushButton (tr ("Save"), this);
  bSaveMessage->setToolTip (tr ("Save messages to text file"));
  connect (bSaveMessage, SIGNAL (clicked ()), this, SLOT (slotSaveMessage ()));
  actions->addWidget (bSaveMessage);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), this);
  closeButton->setToolTip (tr ("Close the message dialog"));
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseMessage ()));
  actions->addWidget (closeButton);


  resize (options->msg_width, options->msg_height);
  move (options->msg_window_x, options->msg_window_y);


  show ();
}



displayMessage::~displayMessage ()
{
}


void 
displayMessage::setMessage ()
{
  if (misc->messages->size ())
    {
      messageBox->clear ();

      for (int32_t i = 0 ; i < misc->messages->size () ; i++)
        {
          QListWidgetItem *cur = new QListWidgetItem (misc->messages->at (i));

          messageBox->addItem (cur);
          messageBox->setCurrentItem (cur);
          messageBox->scrollToItem (cur);
        }
      messageBox->show ();
    }
}



//  Externally callable close function.

void
displayMessage::closeMessage ()
{
  slotCloseMessage ();
}



void
displayMessage::slotCloseMessage ()
{
  //  Use frame geometry to get the absolute x and y.

  QRect tmp = this->frameGeometry ();
  options->msg_window_x = tmp.x ();
  options->msg_window_y = tmp.y ();


  //  Use geometry to get the width and height.

  tmp = this->geometry ();
  options->msg_width = tmp.width ();
  options->msg_height = tmp.height ();


  close ();

  emit closed ();
}



void
displayMessage::slotClearMessage ()
{
  misc->num_messages = 0;
  messageBox->clear ();
  misc->messages->clear ();
}



void
displayMessage::slotSaveMessage ()
{
  FILE *fp = NULL;
  QStringList files, filters;
  QString file;

  QFileDialog *fd = new QFileDialog (this, tr ("pfmEdit3D Save error messages"));
  fd->setViewMode (QFileDialog::List);


  //  Use the directory of the first opened PFM as the default location for the error text file.

  QString dir = QString (pfm_dirname (misc->abe_share->open_args[0].list_path));


  //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
  //  This function is in the nvutility library.

  setSidebarUrls (fd, dir);

  filters << tr ("Text (*.txt)");

  fd->setNameFilters (filters);
  fd->setFileMode (QFileDialog::AnyFile);
  fd->selectNameFilter (tr ("Text (*.txt)"));

  if (fd->exec () == QDialog::Accepted)
    {
      files = fd->selectedFiles ();

      file = files.at (0);


      if (!file.isEmpty())
        {
          //  Add .txt to filename if not there.

          if (!file.endsWith (".txt")) file.append (".txt");
 
          char fname[1024];
          strcpy (fname, file.toLatin1 ());


          if ((fp = fopen (fname, "w")) != NULL)
            {
              for (int32_t i = 0 ; i < misc->messages->size () ; i++)
                {
                  char string[2048];
                  strcpy (string, misc->messages->at (i).toLatin1 ());

                  fprintf (fp, "%s\n", string);
                }

              fclose (fp);
            }

          slotClearMessage ();
          slotCloseMessage ();
        }
    }
}
