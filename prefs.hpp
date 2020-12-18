
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



#ifndef PREFS_H
#define PREFS_H

#include "pfmEdit3DDef.hpp"


class prefs:public QDialog
{
  Q_OBJECT 


    public:

  prefs (QWidget *parent = 0, OPTIONS *op = NULL, MISC *mi = NULL);
  ~prefs ();


 signals:

  void dataChangedSignal ();
  void hotKeyChangedSignal (int32_t i);


 protected:

  ABE_SHARE       mod_share;

  OPTIONS         *options, mod_options;

  MISC            *misc;

  QTabWidget      *prefTab;

  QButtonGroup    *bGrp, *cGrp, *oGrp, *lGrp;

  QDialog         *contoursD, *hotKeyD, *ancillaryProgramD;

  QCheckBox       *kill, *tip, *hsp, *hmv, *scale, *czmilWarnings, *autoScale, *noMaskedCheck, *graySliceCheck, *whiteInvalidCheck, *depthCheck,
                  *elevationCheck, *topCheck, *cloudCheck;

  QGroupBox       *hmvBox;

  QImage          layer1, layer2;

  uint8_t         dataChanged;

  QTableWidget    *contoursTable, *hotKeyTable, *ancillaryProgramTable;

  QPushButton     *bWaveColor[MAX_STACK_POINTS], *bContourColor, *bTrackerColor, *bEditColor, *bMarkerColor,
                  *bBackgroundColor, *bRefColor, *bFeatureColor, *bFeatureInfoColor, *bHighFeatureColor,
                  *bVerFeatureColor, *bScaleColor, *bScaledOffsetZColor, *bRestoreDefaults, *bAncillaryPrograms;

  QPalette        bWavePalette[MAX_STACK_POINTS], bContourPalette, bTrackerPalette, bEditPalette, bMarkerPalette,
                  bBackgroundPalette, bRefPalette, bFeaturePalette, bFeatureInfoPalette, bHighFeaturePalette,
                  bVerFeaturePalette, bScalePalette, bScaledOffsetZPalette;

  QPushButton     *bContourLevels, *bHotKeys;

  QLineEdit       *contourInt, *program[NUMPROGS], *hotkey[NUMPROGS], *actionkey[NUMPROGS];

  QDoubleSpinBox  *offset, *minZ, *rot, *fSize, *featureRadius, *threshold[9];

  QSpinBox        *overlap, *undo, *contourSm, *pointSize, *sparse, *zoomPercent, *hpNeighbors, *hpMultiNeighbors;

  QComboBox       *ihoOrder, *iconSize, *ZFactor, *Width, *hpThreads;

  QCheckBox       *dFilter, *aFilter, *czmilMask[2];

  int32_t         current_command;

  float           save_scale_factor;


  void setFields ();


  protected slots:

  void slotPrefTabChanged (int tab);
  void slotAncillaryPrograms ();
  void slotApplyAncillaryChanges (const QString &text);
  void slotCloseAncillaryProgram ();
  void slotContourColor ();
  void slotTrackerColor ();
  void slotEditColor ();
  void slotMarkerColor ();
  void slotBackgroundColor ();
  void slotRefColor ();
  void slotFeatureColor ();
  void slotFeatureInfoColor ();
  void slotHighFeatureColor ();
  void slotVerFeatureColor ();
  void slotScaleColor ();
  void slotScaledOffsetZColor ();
  void slotColorClicked (int id);
  void slotHelp ();
  void slotRestoreDefaults ();
  void slotHspStateChanged (int state);
  void slotPositionClicked (int id);
  void slotOrientationClicked (int id);
  void slotContourLocationClicked (int id);
  void slotContourLevelsClicked ();
  void slotHotKeys ();
  void slotMinZValueChanged (double value);
  void slotEgmClicked ();
  void slotCloseHotKeys ();
  void slotCloseContours ();
  void slotZFactorChanged (int index);
  void slotApplyPrefs ();
  void slotClosePrefs ();

 private:
};

#endif
