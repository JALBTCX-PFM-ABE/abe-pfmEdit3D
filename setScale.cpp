
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



#include "pfmEdit3D.hpp"


//!  Used to define the color scale boxes on the left of the main window.

void setScale (float attr_min, float attr_max, MISC *misc, OPTIONS *options, uint8_t min_lock, uint8_t max_lock)
{
  //  Set the scale colors for the current range.

  float numshades = (float) (NUMSHADES - 1);
  float inc = 1.0, min_z, max_z, range, value = 0.0;


  int32_t attr_ndx = misc->attrStatNum[options->color_index - PRE_ATTR];


  min_z = attr_min;
  max_z = attr_max;

  range = max_z - min_z;


  //  Check for no range (min will be huge positive, max will be huge negative).

  uint8_t no_range = NVFalse;
  if (min_z > max_z) no_range = NVTrue;


  //  Make sure we don't divide by 0.0

  if (fabs (range) < 0.0000001) range = 1.0;

  inc = range / (float) (NUM_SCALE_LEVELS - 1);


  for (int32_t i = 0 ; i < NUM_SCALE_LEVELS ; i++)
    {
      value = min_z + (float) i * inc;

      int32_t c_index = 0;


      //  Color by Z

      if (!options->color_index)
        {
          c_index = NINT (numshades - ((value) - misc->color_min_z) / misc->color_range_z * numshades);
        }


      //  Color by uncertainty or attribute

      else
        {
          c_index = NINT (numshades - (value - misc->attr_color_min) / misc->attr_color_range * numshades);
        }


      //  Check for out of range values (this is the easy way to work the locked color scale).

      if (c_index < 0) c_index = 0;
      if (c_index > NUMSHADES - 1) c_index = NUMSHADES - 1;


      //  Blank out the scale if there is no range or if we're coloring by line.

      if (no_range || options->color_index == 1)
        {
          misc->scale[i]->setContents (Qt::gray, 999999999999.99);
        }
      else
        {
          misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip ("");


          //  If we've locked the first or last scaleBox, set the flag to draw the lock image.

          int32_t lock = 0;
          QString str;


          //  Top scale box.

          if (!i)
            {
              //  If we're not displaying values as elevations and this is the Z value (options->color_index == 0) we have to flip the tooltips.

              if ((!options->color_index) && options->z_orientation > 0.0)
                {
                  if (min_lock)
                    {
                      lock = 1;

                      str = pfmEdit3D::tr ("Click here to unlock the maximum value (%L1) and/or change the maximum color").arg 
                        (options->max_hsv_value[options->color_index], 0, 'f', 2);
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip (str);
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setWhatsThis (str);
                    }
                  else
                    {
                      lock = -1;

                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip
                        (pfmEdit3D::tr ("Click here to lock the maximum value and/or change the maximum color"));
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setWhatsThis
                        (pfmEdit3D::tr ("Click here to lock the maximum value and/or change the maximum color"));
                    }
                }
              else
                {
                  if (min_lock)
                    {
                      lock = 1;

                      str = pfmEdit3D::tr ("Click here to unlock the minimum value (%L1) and/or change the minimum color").arg 
                        (options->min_hsv_value[options->color_index], 0, 'f', 2);
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip (str);
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setWhatsThis (str);
                    }
                  else
                    {
                      lock = -1;

                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip
                        (pfmEdit3D::tr ("Click here to lock the minimum value and/or change the minimum color"));
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setWhatsThis
                        (pfmEdit3D::tr ("Click here to lock the minimum value and/or change the minimum color"));
                    }
                }
            }


          //  Bottom scale box.

          if (i == NUM_SCALE_LEVELS - 1)
            {
              //  If we're not displaying values as elevations and this is the Z value (options->color_index == 0) we have to flip the tooltips.

              if ((!options->color_index) && options->z_orientation > 0.0)
                {
                  if (max_lock)
                    {
                      lock = 1;

                      str = pfmEdit3D::tr ("Click here to unlock the minimum value (%L1) and/or change the minimum color").arg 
                        (options->min_hsv_value[options->color_index], 0, 'f', 2);
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip (str);
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setWhatsThis (str);
                    }
                  else
                    {
                      lock = -1;

                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip
                        (pfmEdit3D::tr ("Click here to lock the minimum value and/or change the minimum color"));
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setWhatsThis
                        (pfmEdit3D::tr ("Click here to lock the minimum value and/or change the minimum color"));
                    }
                }
              else
                {
                  if (max_lock)
                    {
                      lock = 1;

                      str = pfmEdit3D::tr ("Click here to unlock the maximum value (%L1) and/or change the maximum color").arg 
                        (options->max_hsv_value[options->color_index], 0, 'f', 2);
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip (str);
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setWhatsThis (str);
                    }
                  else
                    {
                      lock = -1;

                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip
                        (pfmEdit3D::tr ("Click here to lock the maximum value and/or change the maximum color"));
                      misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setWhatsThis
                        (pfmEdit3D::tr ("Click here to lock the maximum value and/or change the maximum color"));
                    }
                }
            }


          //  Set tool tips for the time values

          if (options->color_index - PRE_ATTR == misc->time_attr)
            {
              QString tstr;

              int32_t year, jday, hour, minute;
              float second;
              time_t tv_sec = NINT (value * 60.0);

              cvtime (tv_sec, 0, &year, &jday, &hour, &minute, &second);
              tstr.sprintf ("%d-%03d %02d:%02d", year + 1900, jday, hour, minute);

              misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setToolTip (tstr);
            }


          QColor clr;
          clr.setRed (misc->color_array[c_index][0]);
          clr.setGreen (misc->color_array[c_index][1]);
          clr.setBlue (misc->color_array[c_index][2]);
          clr.setAlpha (misc->color_array[c_index][3]);


          if (!options->color_index)
            {
              misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setContents (clr, options->z_orientation * (float) (-value), lock);
            }
          else
            {
              if (options->color_index >= PRE_ATTR && (options->color_index - PRE_ATTR) != misc->time_attr)
                {
                  misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setContents (clr, (float) value, lock, misc->attr_format[0][attr_ndx]);
                }
              else
                {
                  misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setContents (clr, (float) value, lock);
                }
            }
        }
    }
}
