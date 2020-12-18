
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



#ifndef _HPFILTERTHREAD_H_
#define _HPFILTERTHREAD_H_


#include "pfmEdit3D.hpp"


class hpFilterThread:public QThread
{
  Q_OBJECT 


public:

  hpFilterThread (QObject *parent = 0);
  ~hpFilterThread ();

  void filter (OPTIONS *op = NULL, MISC *mi = NULL, FILTER_BIN_DATA **bd = NULL, FILTER_POS_DATA *pd = NULL, int32_t rws = 0, int32_t cls = 0,
               int32_t w = 0, int32_t h = 0, int32_t rw = 0, int32_t cl = 0, std::vector<int32_t> *kl = NULL, int32_t *kc = NULL,
               std::vector<FEATURE_RECORD> *fr = NULL, int32_t fc = 0);


signals:

//void completed (int32_t pass);


protected:


  QMutex           mutex;

  OPTIONS          *l_options;

  MISC             *l_misc;

  FILTER_BIN_DATA  **l_bin_data;

  FILTER_POS_DATA  *l_pos;

  std::vector<int32_t> *l_kill_list;

  std::vector<FEATURE_RECORD> *l_feature;

  int32_t          l_rows, l_cols, l_width, l_height, l_row, l_col, *l_kill_count, l_feature_count;

  void             run ();


protected slots:

private:
};

#endif
