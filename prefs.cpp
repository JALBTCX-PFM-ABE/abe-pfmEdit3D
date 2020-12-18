
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



#include "prefs.hpp"
#include "prefsHelp.hpp"


//!  This is the preferences dialog.

prefs::prefs (QWidget *parent, OPTIONS *op, MISC *mi):
  QDialog (parent, (Qt::WindowFlags) Qt::WA_DeleteOnClose)
{
  options = op;
  misc = mi;

  mod_share = *misc->abe_share;
  mod_options = *op;

  setModal (true);

  ancillaryProgramD = hotKeyD = NULL;


  dataChanged = NVFalse;


  setWindowTitle (tr ("pfmEdit3D Preferences"));

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  QGroupBox *conBox = new QGroupBox (this);
  QHBoxLayout *conBoxLayout = new QHBoxLayout;
  conBox->setLayout (conBoxLayout);
  QVBoxLayout *conBoxLeftLayout = new QVBoxLayout;
  QVBoxLayout *conBoxRightLayout = new QVBoxLayout;
  conBoxLayout->addLayout (conBoxLeftLayout);
  conBoxLayout->addLayout (conBoxRightLayout);


  oGrp = new QButtonGroup (this);
  oGrp->setExclusive (true);
  connect (oGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotOrientationClicked (int)));

  QGroupBox *orientBox = new QGroupBox (tr ("Z Orientation"), this);
  QHBoxLayout *orientBoxLayout = new QHBoxLayout;
  orientBox->setLayout (orientBoxLayout);
  orientBox->setWhatsThis (orientBoxText);

  depthCheck = new QCheckBox (tr ("Depth"), orientBox);
  depthCheck->setToolTip (tr ("Z values displayed as depths"));
  depthCheck->setWhatsThis (depthCheckText);
  oGrp->addButton (depthCheck, 0);
  orientBoxLayout->addWidget (depthCheck);

  elevationCheck = new QCheckBox (tr ("Elevation"), orientBox);
  elevationCheck->setToolTip (tr ("Z values displayed as elevations"));
  elevationCheck->setWhatsThis (elevationCheckText);
  oGrp->addButton (elevationCheck, 1);
  orientBoxLayout->addWidget (elevationCheck);

  conBoxLeftLayout->addWidget (orientBox);


  QGroupBox *zBox = new QGroupBox (tr ("Z Scale"), this);
  QHBoxLayout *zBoxLayout = new QHBoxLayout;
  zBox->setLayout (zBoxLayout);
  ZFactor = new QComboBox (zBox);
  ZFactor->setToolTip (tr ("Change the Z scale factor"));
  ZFactor->setWhatsThis (ZFactorText);
  ZFactor->setEditable (false);
  ZFactor->addItem (tr ("%L1 (no scaling)").arg (1.00000, 0, 'f', 4));
  ZFactor->addItem (tr ("%L1 (meters to feet)").arg (3.28084, 0, 'f', 4));
  ZFactor->addItem (tr ("%L1 (feet to meters)").arg (0.34080, 0, 'f', 4));
  ZFactor->addItem (tr ("%L1 (meters to fathoms)").arg (0.54681, 0, 'f', 4));
  ZFactor->addItem (tr ("%L1 (fathoms to meters)").arg (1.82880, 0, 'f', 4));
  ZFactor->addItem (tr ("%L1 (meters to fathoms, at 4800 ft/sec)").arg (0.53333, 0, 'f', 4));
  connect (ZFactor, SIGNAL (currentIndexChanged (int)), this, SLOT (slotZFactorChanged (int)));
  zBoxLayout->addWidget (ZFactor);
  conBoxLeftLayout->addWidget (zBox);


  QGroupBox *offBox = new QGroupBox (tr ("Z Offset"), this);
  QHBoxLayout *offBoxLayout = new QHBoxLayout;
  offBox->setLayout (offBoxLayout);
  offset = new QDoubleSpinBox (offBox);
  offset->setDecimals (2);
  offset->setRange (-1000.0, 1000.0);
  offset->setSingleStep (10.0);
  offset->setToolTip (tr ("Change the contour/Z offset value"));
  offset->setWhatsThis (offsetText);
  offBoxLayout->addWidget (offset);

  QPushButton *egmButton = new QPushButton (tr ("EGM08"));
  egmButton->setToolTip (tr ("Get the ellipsoid to geoid datum offset from EGM08 model"));
  egmButton->setWhatsThis (egmText);
  connect (egmButton, SIGNAL (clicked ()), this, SLOT (slotEgmClicked (void)));
  offBoxLayout->addWidget (egmButton);
  conBoxLeftLayout->addWidget (offBox);


  QGroupBox *zoomBox = new QGroupBox (tr ("Zoom percentage"), this);
  QHBoxLayout *zoomBoxLayout = new QHBoxLayout;
  zoomBox->setLayout (zoomBoxLayout);
  zoomPercent = new QSpinBox (zoomBox);
  zoomPercent->setRange (5, 50);
  zoomPercent->setSingleStep (5);
  zoomPercent->setToolTip (tr ("Change the zoom in/out percentage (10 - 50)"));
  zoomPercent->setWhatsThis (zoomPercentText);
  zoomBoxLayout->addWidget (zoomPercent);
  conBoxLeftLayout->addWidget (zoomBox);


  QGroupBox *pixBox = new QGroupBox (tr ("Point size"), this);
  QHBoxLayout *pixBoxLayout = new QHBoxLayout;
  pixBox->setLayout (pixBoxLayout);
  pointSize = new QSpinBox (pixBox);
  pointSize->setRange (2, 10);
  pointSize->setSingleStep (1);
  pointSize->setToolTip (tr ("Change the point size (pixels) (2 - 10)"));
  pointSize->setWhatsThis (pointSizeText);
  pixBoxLayout->addWidget (pointSize);
  conBoxLeftLayout->addWidget (pixBox);


  QGroupBox *intBox = new QGroupBox (tr ("Contour interval"), this);
  QHBoxLayout *intBoxLayout = new QHBoxLayout;
  intBox->setLayout (intBoxLayout);

  contourInt = new QLineEdit (intBox);
  contourInt->setToolTip (tr ("Change the contour interval"));
  contourInt->setWhatsThis (contourIntText);
  intBoxLayout->addWidget (contourInt);

  bContourLevels = new QPushButton (tr ("Set Contour Levels"), intBox);
  bContourLevels->setToolTip (tr ("Set non-uniform individual contour intervals"));
  bContourLevels->setWhatsThis (contourLevelsText);
  connect (bContourLevels, SIGNAL (clicked ()), this, SLOT (slotContourLevelsClicked ()));
  intBoxLayout->addWidget (bContourLevels);
  conBoxRightLayout->addWidget (intBox);


  QGroupBox *smBox = new QGroupBox (tr ("Contour smoothing"), this);
  QHBoxLayout *smBoxLayout = new QHBoxLayout;
  smBox->setLayout (smBoxLayout);
  contourSm = new QSpinBox (smBox);
  contourSm->setRange (0, 10);
  contourSm->setSingleStep (1);
  contourSm->setToolTip (tr ("Change the contour smoothing factor (0 - 10)"));
  contourSm->setWhatsThis (contourSmText);
  smBoxLayout->addWidget (contourSm);
  conBoxRightLayout->addWidget (smBox);


  QGroupBox *widBox = new QGroupBox (tr ("Contour width"), this);
  QHBoxLayout *widBoxLayout = new QHBoxLayout;
  widBox->setLayout (widBoxLayout);
  Width = new QComboBox (widBox);
  Width->setToolTip (tr ("Change the contour line width/thickness (pixels)"));
  Width->setWhatsThis (WidthText);
  Width->setEditable (true);
  Width->addItem ("1");
  Width->addItem ("2");
  Width->addItem ("3");
  widBoxLayout->addWidget (Width);
  conBoxRightLayout->addWidget (widBox);


  lGrp = new QButtonGroup (this);
  lGrp->setExclusive (true);
  connect (lGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotContourLocationClicked (int)));

  QGroupBox *locBox = new QGroupBox (tr ("Contour location"), this);
  QHBoxLayout *locBoxLayout = new QHBoxLayout;
  locBox->setLayout (locBoxLayout);
  locBox->setWhatsThis (locBoxText);

  topCheck = new QCheckBox (tr ("Top"), locBox);
  topCheck->setToolTip (tr ("Contours displayed at the top of the 3D space"));
  topCheck->setWhatsThis (topCheckText);
  lGrp->addButton (topCheck, 0);
  locBoxLayout->addWidget (topCheck);

  cloudCheck = new QCheckBox (tr ("Point cloud"), locBox);
  cloudCheck->setToolTip (tr ("Contours displayed in the point cloud"));
  cloudCheck->setWhatsThis (cloudCheckText);
  lGrp->addButton (cloudCheck, 1);
  locBoxLayout->addWidget (cloudCheck);

  conBoxRightLayout->addWidget (locBox);


  prefTab = new QTabWidget ();
  prefTab->setTabPosition (QTabWidget::North);

  prefTab->addTab (conBox, tr ("Depth/Elevation"));
  prefTab->setTabToolTip (0, tr ("Set the depth/elevation options"));
  prefTab->setTabWhatsThis (0, conText);


  QGroupBox *cbox = new QGroupBox (tr ("Colors"), this);
  QHBoxLayout *cboxLayout = new QHBoxLayout;
  cbox->setLayout (cboxLayout);
  QVBoxLayout *cboxLeftLayout = new QVBoxLayout;
  QVBoxLayout *cboxRightLayout = new QVBoxLayout;
  cboxLayout->addLayout (cboxLeftLayout);
  cboxLayout->addLayout (cboxRightLayout);


  bTrackerColor = new QPushButton (tr ("Tracker"), this);
  bTrackerColor->setToolTip (tr ("Change tracker color"));
  bTrackerColor->setWhatsThis (trackerColorText);
  bTrackerPalette = bTrackerColor->palette ();
  connect (bTrackerColor, SIGNAL (clicked ()), this, SLOT (slotTrackerColor ()));
  cboxLeftLayout->addWidget (bTrackerColor);


  bEditColor = new QPushButton (tr ("Edit"), this);
  bEditColor->setToolTip (tr ("Change edit color"));
  bEditColor->setWhatsThis (editColorText);
  bEditPalette = bEditColor->palette ();
  connect (bEditColor, SIGNAL (clicked ()), this, SLOT (slotEditColor ()));
  cboxLeftLayout->addWidget (bEditColor);


  bMarkerColor = new QPushButton (tr ("Marker"), this);
  bMarkerColor->setToolTip (tr ("Change marker color"));
  bMarkerColor->setWhatsThis (markerColorText);
  bMarkerPalette = bMarkerColor->palette ();
  connect (bMarkerColor, SIGNAL (clicked ()), this, SLOT (slotMarkerColor ()));
  cboxLeftLayout->addWidget (bMarkerColor);


  bFeatureColor = new QPushButton (tr ("Feature"), this);
  bFeatureColor->setToolTip (tr ("Change feature color"));
  bFeatureColor->setWhatsThis (featureColorText);
  bFeaturePalette = bFeatureColor->palette ();
  connect (bFeatureColor, SIGNAL (clicked ()), this, SLOT (slotFeatureColor ()));
  cboxLeftLayout->addWidget (bFeatureColor);


  bFeatureInfoColor = new QPushButton (tr ("Feature information"), this);
  bFeatureInfoColor->setToolTip (tr ("Change feature information text color"));
  bFeatureInfoColor->setWhatsThis (featureInfoColorText);
  bFeatureInfoPalette = bFeatureInfoColor->palette ();
  connect (bFeatureInfoColor, SIGNAL (clicked ()), this, SLOT (slotFeatureInfoColor ()));
  cboxLeftLayout->addWidget (bFeatureInfoColor);


  bHighFeatureColor = new QPushButton (tr ("Highlighted Feature"), this);
  bHighFeatureColor->setToolTip (tr ("Change highlighted feature color"));
  bHighFeatureColor->setWhatsThis (highFeatureColorText);
  bHighFeaturePalette = bHighFeatureColor->palette ();
  connect (bHighFeatureColor, SIGNAL (clicked ()), this, SLOT (slotHighFeatureColor ()));
  cboxLeftLayout->addWidget (bHighFeatureColor);


  bVerFeatureColor = new QPushButton (tr ("Verified Feature"), this);
  bVerFeatureColor->setToolTip (tr ("Change verified feature color"));
  bVerFeatureColor->setWhatsThis (verFeatureColorText);
  bVerFeaturePalette = bVerFeatureColor->palette ();
  connect (bVerFeatureColor, SIGNAL (clicked ()), this, SLOT (slotVerFeatureColor ()));
  cboxLeftLayout->addWidget (bVerFeatureColor);


  bContourColor = new QPushButton (tr ("Contour"), this);
  bContourColor->setToolTip (tr ("Change contour color"));
  bContourColor->setWhatsThis (contourColorText);
  bContourPalette = bContourColor->palette ();
  connect (bContourColor, SIGNAL (clicked ()), this, SLOT (slotContourColor ()));
  cboxLeftLayout->addWidget (bContourColor);


  bBackgroundColor = new QPushButton (tr ("Background"), this);
  bBackgroundColor->setToolTip (tr ("Change display background color"));
  bBackgroundColor->setWhatsThis (backgroundColorText);
  bBackgroundPalette = bBackgroundColor->palette ();
  connect (bBackgroundColor, SIGNAL (clicked ()), this, SLOT (slotBackgroundColor ()));
  cboxLeftLayout->addWidget (bBackgroundColor);


  bRefColor = new QPushButton (tr ("Reference/NULL"), this);
  bRefColor->setToolTip (tr ("Change display reference and NULL data color"));
  bRefColor->setWhatsThis (refColorText);
  bRefPalette = bRefColor->palette ();
  connect (bRefColor, SIGNAL (clicked ()), this, SLOT (slotRefColor ()));
  cboxLeftLayout->addWidget (bRefColor);


  bScaleColor = new QPushButton (tr ("Scale"), this);
  bScaleColor->setToolTip (tr ("Change scale color"));
  bScaleColor->setWhatsThis (scaleColorText);
  bScalePalette = bScaleColor->palette ();
  connect (bScaleColor, SIGNAL (clicked ()), this, SLOT (slotScaleColor ()));
  cboxLeftLayout->addWidget (bScaleColor);


  bScaledOffsetZColor = new QPushButton (tr ("Scaled/Offset Z"), this);
  bScaledOffsetZColor->setToolTip (tr ("Change color for scaled and/or offset Z values"));
  bScaledOffsetZColor->setWhatsThis (scaledOffsetZColorText);
  bScaledOffsetZPalette = bScaledOffsetZColor->palette ();
  connect (bScaledOffsetZColor, SIGNAL (clicked ()), this, SLOT (slotScaledOffsetZColor ()));
  cboxRightLayout->addWidget (bScaledOffsetZColor);


  cGrp = new QButtonGroup (this);
  connect (cGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotColorClicked (int)));


  for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++)
    {
      QString string = tr ("Cursor %1").arg (i, 2, 10, QChar('0'));
      bWaveColor[i] = new QPushButton (string, this);
      string.prepend ("Change color for ");
      bWaveColor[i]->setToolTip (string);
      bWaveColor[i]->setWhatsThis (waveColorText);
      bWavePalette[i] = bWaveColor[i]->palette ();

      cGrp->addButton (bWaveColor[i], i);

      cboxRightLayout->addWidget (bWaveColor[i]);
    }


  //  Check box for coloring and scaling NOT using masked data.

  noMaskedCheck = new QCheckBox (tr ("No hidden data scaling or coloring"), cbox);
  noMaskedCheck->setToolTip (tr ("Do not used hidden (masked) data for scaling or coloring"));
  noMaskedCheck->setWhatsThis (noMaskedCheckText);
  cboxLeftLayout->addWidget (noMaskedCheck);


  //  Check box for gray data outside of slice.

  graySliceCheck = new QCheckBox (tr ("Data outside slice set to gray"), cbox);
  graySliceCheck->setToolTip (tr ("Set all data outside of the slice area to gray, semi-transparent"));
  graySliceCheck->setWhatsThis (graySliceCheckText);
  cboxRightLayout->addWidget (graySliceCheck);


  //  Check box for "Edit" color invalid data.

  whiteInvalidCheck = new QCheckBox (tr ("Set invalid data to the Edit color"), cbox);
  whiteInvalidCheck->setToolTip (tr ("Set all displayed invalid data to the <b>Edit</b> color"));
  whiteInvalidCheck->setWhatsThis (whiteInvalidCheckText);
  cboxRightLayout->addWidget (whiteInvalidCheck);


  cboxLeftLayout->addStretch (1);
  cboxRightLayout->addStretch (1);


  prefTab->addTab (cbox, tr ("Colors"));
  prefTab->setTabToolTip (1, tr ("Set the display colors"));
  prefTab->setTabWhatsThis (1, colorText);


  QGroupBox *filterBox = new QGroupBox (this);
  QVBoxLayout *filterBoxLayout = new QVBoxLayout;
  filterBox->setLayout (filterBoxLayout);


  QGroupBox *dBox = new QGroupBox (tr ("Area-based filter deep filter only"), this);
  dBox->setToolTip (tr ("Toggle filtering of deep values only (for area-based statistical filter)"));
  QHBoxLayout *dBoxLayout = new QHBoxLayout;
  dBox->setLayout (dBoxLayout);
  dFilter = new QCheckBox (dBox);
  dFilter->setToolTip (tr ("Toggle filtering of deep values only (for area-based statistical filter)"));
  dFilter->setWhatsThis (dFilterText);
  dBoxLayout->addWidget (dFilter);
  filterBoxLayout->addWidget (dBox);

  QGroupBox *rBox = new QGroupBox (tr ("Filter feature exclusion radius"), this);
  QHBoxLayout *rBoxLayout = new QHBoxLayout;
  rBox->setLayout (rBoxLayout);
  featureRadius = new QDoubleSpinBox (rBox);
  featureRadius->setDecimals (1);
  featureRadius->setRange (0.0, 100.0);
  featureRadius->setSingleStep (1.0);
  featureRadius->setToolTip (tr ("Change the feature exclusion radius (m) for all filters (except attribute)"));
  featureRadius->setWhatsThis (featureRadiusText);
  rBoxLayout->addWidget (featureRadius);
  filterBoxLayout->addWidget (rBox);


  QGroupBox *hptBox = new QGroupBox (tr ("Number of filter threads"), this);
  QHBoxLayout *hptBoxLayout = new QHBoxLayout;
  hptBox->setLayout (hptBoxLayout);
  hpThreads = new QComboBox (hptBox);
  hpThreads->setToolTip (tr ("Change the number of threads used by the Hockey Puck and AVT filters"));
  hpThreads->setWhatsThis (hpThreadsText);
  hpThreads->setEditable (false);
  hpThreads->addItem ("1");
  hpThreads->addItem ("4");
  hpThreads->addItem ("16");
  hptBoxLayout->addWidget (hpThreads);
  filterBoxLayout->addWidget (hptBox);


  QGroupBox *aBox = new QGroupBox (tr ("Automatic filter mask on exit"), this);
  aBox->setToolTip (tr ("Toggle automatic filter masking in pfmView on exit from pfmEdit3D"));
  QHBoxLayout *aBoxLayout = new QHBoxLayout;
  aBox->setLayout (aBoxLayout);
  aFilter = new QCheckBox (aBox);
  aFilter->setToolTip (tr ("Toggle automatic filter masking in pfmView on exit from pfmEdit3D"));
  aFilter->setWhatsThis (aFilterText);
  aBoxLayout->addWidget (aFilter);
  filterBoxLayout->addWidget (aBox);

  filterBoxLayout->addStretch (1);


  prefTab->addTab (filterBox, tr ("Filter"));
  prefTab->setTabToolTip (2, tr ("Set the filter options"));
  prefTab->setTabWhatsThis (2, filtText);


  QGroupBox *miscBox = new QGroupBox (this);
  QHBoxLayout *miscBoxLayout = new QHBoxLayout;
  miscBox->setLayout (miscBoxLayout);
  QVBoxLayout *miscBoxLeftLayout = new QVBoxLayout;
  QVBoxLayout *miscBoxRightLayout = new QVBoxLayout;
  miscBoxLayout->addLayout (miscBoxLeftLayout);
  miscBoxLayout->addLayout (miscBoxRightLayout);


  QGroupBox *minBox = new QGroupBox (tr ("Minimum Z window size"), this);
  QHBoxLayout *minBoxLayout = new QHBoxLayout;
  minBox->setLayout (minBoxLayout);

  ihoOrder = new QComboBox (minBox);
  ihoOrder->setToolTip (tr ("Use IHO order to define minimum Z window size"));
  ihoOrder->setWhatsThis (ihoOrderText);
  ihoOrder->setEditable (true);
  ihoOrder->addItem ("Set Manually ->");
  ihoOrder->addItem ("IHO Special Order");
  ihoOrder->addItem ("IHO Order 1a/1b");
  ihoOrder->addItem ("IHO Order 2");
  minBoxLayout->addWidget (ihoOrder);


  minZ = new QDoubleSpinBox (minBox);
  minZ->setDecimals (1);
  minZ->setRange (0.1, 100.0);
  minZ->setSingleStep (1.0);
  minZ->setToolTip (tr ("Manually set the minimum Z window size"));
  minZ->setWhatsThis (minZText);
  minBoxLayout->addWidget (minZ);

  miscBoxLeftLayout->addWidget (minBox);


  QGroupBox *sparseBox = new QGroupBox (tr ("Sparse point limit"), this);
  QHBoxLayout *sparseBoxLayout = new QHBoxLayout;
  sparseBox->setLayout (sparseBoxLayout);

  sparse = new QSpinBox (this);
  sparse->setRange (50000, 1000000);
  sparse->setSingleStep (1000);
  sparse->setToolTip (tr ("Set the point limit at which we begin to plot sparse points"));
  sparse->setWhatsThis (sparseText);
  sparseBoxLayout->addWidget (sparse);

  miscBoxLeftLayout->addWidget (sparseBox);


  QGroupBox *overlapBox = new QGroupBox (tr ("Overlap %"), this);
  QHBoxLayout *overlapBoxLayout = new QHBoxLayout;
  overlapBox->setLayout (overlapBoxLayout);
  overlap = new QSpinBox (overlapBox);
  overlap->setRange (1, 50);
  overlap->setSingleStep (1);
  overlap->setToolTip (tr ("Change the window overlap percentage (1 - 50)"));
  overlap->setWhatsThis (overlapText);
  overlapBoxLayout->addWidget (overlap);
  miscBoxLeftLayout->addWidget (overlapBox);


  QGroupBox *rotBox = new QGroupBox (tr ("Rotation"), this);
  QHBoxLayout *rotBoxLayout = new QHBoxLayout;
  rotBox->setLayout (rotBoxLayout);
  rot = new QDoubleSpinBox (rotBox);
  rot->setDecimals (1);
  rot->setRange (1.0, 30.0);
  rot->setSingleStep (1.0);
  rot->setToolTip (tr ("Change the view rotation increment (1.0 - 30.0)"));
  rot->setWhatsThis (rotText);
  rotBoxLayout->addWidget (rot);
  miscBoxLeftLayout->addWidget (rotBox);


  QGroupBox *fSizeBox = new QGroupBox (tr ("Feature size"), this);
  QHBoxLayout *fSizeBoxLayout = new QHBoxLayout;
  fSizeBox->setLayout (fSizeBoxLayout);
  fSize = new QDoubleSpinBox (this);
  fSize->setDecimals (3);
  fSize->setRange (0.001, 0.01);
  fSize->setSingleStep (0.001);
  fSize->setValue (options->feature_size);
  fSize->setToolTip (tr ("Change the feature marker size (0.001 - 0.01)"));
  fSize->setWhatsThis (fSizeText);
  fSizeBoxLayout->addWidget (fSize);
  miscBoxLeftLayout->addWidget (fSizeBox);


  QGroupBox *undoBox = new QGroupBox (tr ("Undo levels"), this);
  QHBoxLayout *undoBoxLayout = new QHBoxLayout;
  undoBox->setLayout (undoBoxLayout);
  undo = new QSpinBox (undoBox);
  undo->setRange (100, 1000000);
  undo->setSingleStep (100);
  undo->setToolTip (tr ("Change the maximum number of undo levels (100 - 1,000,000)"));
  undo->setWhatsThis (prefsUndoText);
  undoBoxLayout->addWidget (undo);
  miscBoxLeftLayout->addWidget (undoBox);


  QGroupBox *autoScaleBox = new QGroupBox (tr ("Auto scale"), this);
  autoScaleBox->setToolTip (tr ("If checked, data will be auto-scaled when redrawn"));
  QHBoxLayout *autoScaleBoxLayout = new QHBoxLayout;
  autoScaleBox->setLayout (autoScaleBoxLayout);
  autoScale = new QCheckBox (this);
  autoScale->setToolTip (tr ("If checked, data will be auto-scaled when redrawn"));
  autoScale->setWhatsThis (autoScaleText);
  autoScaleBoxLayout->addWidget (autoScale);
  miscBoxRightLayout->addWidget (autoScaleBox);


  QGroupBox *scaleBox = new QGroupBox (tr ("Draw scale"), this);
  scaleBox->setToolTip (tr ("If checked, the scale will be drawn on the screen"));
  QHBoxLayout *scaleBoxLayout = new QHBoxLayout;
  scaleBox->setLayout (scaleBoxLayout);
  scale = new QCheckBox (this);
  scale->setToolTip (tr ("If checked, the scale will be drawn on the screen"));
  scale->setWhatsThis (scaleText);
  scaleBoxLayout->addWidget (scale);
  miscBoxRightLayout->addWidget (scaleBox);


  QGroupBox *killBox = new QGroupBox (tr ("Kill/Respawn"), this);
  killBox->setToolTip (tr ("If checked, ancillary programs will exit/respawn on close/open"));
  QHBoxLayout *killBoxLayout = new QHBoxLayout;
  killBox->setLayout (killBoxLayout);
  kill = new QCheckBox (this);
  kill->setToolTip (tr ("If checked, ancillary programs will exit/respawn on close/open"));
  kill->setWhatsThis (killText);
  killBoxLayout->addWidget (kill);
  miscBoxRightLayout->addWidget (killBox);


  QGroupBox *tipBox = new QGroupBox (tr ("Highlighted point tooltip"), this);
  tipBox->setToolTip (tr ("If checked, tool tip will be displayed when highlighted points are present"));
  QHBoxLayout *tipBoxLayout = new QHBoxLayout;
  tipBox->setLayout (tipBoxLayout);
  tip = new QCheckBox (this);
  tip->setToolTip (tr ("If checked, tool tip will be displayed when highlighted points are present"));
  tip->setWhatsThis (tipText);
  tipBoxLayout->addWidget (tip);
  miscBoxRightLayout->addWidget (tipBox);


  QGroupBox *hspBox = new QGroupBox (tr ("Make hide by attribute persistent"), this);
  hspBox->setToolTip (tr ("If checked, hide options will remain in effect between edit sessions"));
  QHBoxLayout *hspBoxLayout = new QHBoxLayout;
  hspBox->setLayout (hspBoxLayout);
  hsp = new QCheckBox (this);
  hsp->setToolTip (tr ("If checked, hide options will remain in effect between edit sessions"));
  hsp->setWhatsThis (hspText);
  connect (hsp, SIGNAL (stateChanged (int)), this, SLOT (slotHspStateChanged (int)));
  hspBoxLayout->addWidget (hsp);
  miscBoxRightLayout->addWidget (hspBox);


  hmvBox = new QGroupBox (tr ("Make Hidden Data tab visible"), this);
  hmvBox->setToolTip (tr ("If checked and hide is persistent and data is hidden, the Hidden Data tab will be visible on startup"));
  hmvBox->setWhatsThis (hmvText);
  QHBoxLayout *hmvBoxLayout = new QHBoxLayout;
  hmvBox->setLayout (hmvBoxLayout);
  hmv = new QCheckBox (this);
  hmv->setToolTip (tr ("If checked and hide is persistent and data is hidden, the Hidden Data tab will be visible on startup"));
  hmv->setWhatsThis (hmvText);
  hmvBoxLayout->addWidget (hmv);
  miscBoxRightLayout->addWidget (hmvBox);


  miscBoxLeftLayout->addStretch (1);
  miscBoxRightLayout->addStretch (1);


  prefTab->addTab (miscBox, tr ("Miscellaneous"));
  prefTab->setTabToolTip (3, tr ("Set miscellaneous options"));
  prefTab->setTabWhatsThis (3, miscText);



  QRadioButton *hdms = new QRadioButton (tr ("Hemisphere Degrees Minutes Seconds.decimal"));
  QRadioButton *hdm_ = new QRadioButton (tr ("Hemisphere Degrees Minutes.decimal"));
  QRadioButton *hd__ = new QRadioButton (tr ("Hemisphere Degrees.decimal"));
  QRadioButton *sdms = new QRadioButton (tr ("+/-Degrees Minutes Seconds.decimal"));
  QRadioButton *sdm_ = new QRadioButton (tr ("+/-Degrees Minutes.decimal"));
  QRadioButton *sd__ = new QRadioButton (tr ("+/-Degrees.decimal"));

  QGroupBox *formBox = new QGroupBox (this);
  QVBoxLayout *formBoxLayout = new QVBoxLayout;
  formBox->setLayout (formBoxLayout);

  bGrp = new QButtonGroup (this);
  bGrp->setExclusive (true);
  connect (bGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotPositionClicked (int)));

  bGrp->addButton (hdms, 0);
  bGrp->addButton (hdm_, 1);
  bGrp->addButton (hd__, 2);
  bGrp->addButton (sdms, 3);
  bGrp->addButton (sdm_, 4);
  bGrp->addButton (sd__, 5);

  formBoxLayout->addWidget (hdms);
  formBoxLayout->addWidget (hdm_);
  formBoxLayout->addWidget (hd__);
  formBoxLayout->addWidget (sdms);
  formBoxLayout->addWidget (sdm_);
  formBoxLayout->addWidget (sd__);

  formBoxLayout->addStretch (1);


  prefTab->addTab (formBox, tr ("Position Format"));
  prefTab->setTabToolTip (4, tr ("Set the position display format"));
  prefTab->setTabWhatsThis (4, bGrpText);


  QGroupBox *kbox = new QGroupBox (this);
  QVBoxLayout *kboxLayout = new QVBoxLayout;
  kbox->setLayout (kboxLayout);


  bHotKeys = new QPushButton (tr ("Main Buttons"), this);
  bHotKeys->setToolTip (tr ("Change hot key sequences for buttons"));
  bHotKeys->setWhatsThis (hotKeysText);
  connect (bHotKeys, SIGNAL (clicked ()), this, SLOT (slotHotKeys ()));
  kboxLayout->addWidget (bHotKeys);


  QGroupBox *iconBox = new QGroupBox (tr ("Main Button Icon Size"), this);
  QHBoxLayout *iconBoxLayout = new QHBoxLayout;
  iconBox->setLayout (iconBoxLayout);

  iconSize = new QComboBox (iconBox);
  iconSize->setToolTip (tr ("Set the size (in pixels) of the main button icons"));
  iconSize->setWhatsThis (iconSizeText);
  iconSize->setEditable (false);
  iconSize->addItem ("16");
  iconSize->addItem ("20");
  iconSize->addItem ("24");
  iconSize->addItem ("28");
  iconSize->addItem ("32");
  iconBoxLayout->addWidget (iconSize);


  kboxLayout->addWidget (iconBox);


  bAncillaryPrograms = new QPushButton (tr ("Ancillary Programs"), this);
  bAncillaryPrograms->setToolTip (tr ("Change ancillary program hot keys and action keys"));
  bAncillaryPrograms->setWhatsThis (ancillaryProgramsText);
  connect (bAncillaryPrograms, SIGNAL (clicked ()), this, SLOT (slotAncillaryPrograms ()));
  kboxLayout->addWidget (bAncillaryPrograms);


  kboxLayout->addStretch (1);


  prefTab->addTab (kbox, tr ("Hot Keys"));
  prefTab->setTabToolTip (5, tr ("Set hot keys"));
  prefTab->setTabWhatsThis (5, kboxText);


  QGroupBox *czmilBox = new QGroupBox (this);
  QVBoxLayout *czmilBoxLayout = new QVBoxLayout;
  czmilBox->setLayout (czmilBoxLayout);

  QGroupBox *czmilWarningsBox = new QGroupBox (tr ("Suppress czmilReprocess warnings"), this);
  czmilWarningsBox->setToolTip (tr ("If checked, czmilReprocess warnings will not be displayed"));
  QHBoxLayout *czmilWarningsBoxLayout = new QHBoxLayout;
  czmilWarningsBox->setLayout (czmilWarningsBoxLayout);
  czmilWarnings = new QCheckBox (this);
  czmilWarnings->setToolTip (tr ("If checked, czmilReprocess warnings will not be displayed"));
  czmilWarnings->setWhatsThis (czmilWarningsText);
  czmilWarningsBoxLayout->addWidget (czmilWarnings);
  czmilBoxLayout->addWidget (czmilWarningsBox);


  QGroupBox *czmilMaskBox = new QGroupBox (tr ("CZMIL reprocessing data types"), this);
  QHBoxLayout *czmilMaskBoxLayout = new QHBoxLayout;
  czmilMaskBox->setLayout (czmilMaskBoxLayout);

  QLabel *mLabel[3];

  for (int32_t i = 0 ; i < 2 ; i++)
    {
      QGroupBox *mBox = new QGroupBox (this);
      QHBoxLayout *mBoxLayout = new QHBoxLayout;
      mBox->setLayout (mBoxLayout);

      char proc_mode_string[1024];
      int32_t mode = 0;

      switch (i)
        {
        case 0:
          mode = CZMIL_OPTECH_CLASS_LAND;
          break;

        case 1:
          mode = CZMIL_OPTECH_CLASS_WATER;
          break;
        }

      czmil_get_proc_mode_string (mode, proc_mode_string);

      QString tip = tr ("Select/deselect CZMIL reprocessing for %1 processed data").arg (proc_mode_string);

      mLabel[i] = new QLabel (proc_mode_string, this);
      mLabel[i]->setToolTip (tip);
      mLabel[i]->setWhatsThis (tip);
      mBoxLayout->addWidget (mLabel[i]);


      czmilMask[i] = new QCheckBox (this);
      czmilMask[i]->setToolTip (tip);
      czmilMask[i]->setWhatsThis (tip);
      mBoxLayout->addWidget (czmilMask[i]);

      czmilMaskBoxLayout->addWidget (mBox);
    }

  czmilBoxLayout->addWidget (czmilMaskBox);


  QGroupBox *threshBox = new QGroupBox (tr ("Probability of detection thresholds"), this);
  QHBoxLayout *threshBoxLayout = new QHBoxLayout;
  threshBox->setLayout (threshBoxLayout);
  QVBoxLayout *threshBoxLeftLayout = new QVBoxLayout;
  QVBoxLayout *threshBoxRightLayout = new QVBoxLayout;
  threshBoxLayout->addLayout (threshBoxLeftLayout);
  threshBoxLayout->addLayout (threshBoxRightLayout);
  czmilBoxLayout->addWidget (threshBox, 1);

  QLabel *tLabel[9];

  for (int32_t i = 0 ; i < 9 ; i++)
    {
      QGroupBox *tBox = new QGroupBox (this);
      QHBoxLayout *tBoxLayout = new QHBoxLayout;
      tBox->setLayout (tBoxLayout);

      char channel_string[1024];
      czmil_get_channel_string (i, channel_string);

      QString t_label = QString (channel_string);
      QString tip = tr ("Set the probability of detection threshold for %1").arg (t_label);

      QString thresholdText =
        tr ("Set the probability of detection threshold for %1.  The maximum (and default) value is 0.9999.  This is used for czmilReprocess").arg (t_label);


      tLabel[i] = new QLabel (channel_string, this);
      tLabel[i]->setToolTip (tip);
      tLabel[i]->setWhatsThis (thresholdText);
      tBoxLayout->addWidget (tLabel[i]);

      threshold[i] = new QDoubleSpinBox (tBox);
      threshold[i]->setDecimals (4);
      threshold[i]->setRange (0.0001, 0.9999);
      threshold[i]->setSingleStep (0.001);
      threshold[i]->setToolTip (tip);
      threshold[i]->setWhatsThis (thresholdText);
      tBoxLayout->addWidget (threshold[i]);

      if (i < 4)
        {
          threshBoxLeftLayout->addWidget (tBox);
        }
      else
        {
          threshBoxRightLayout->addWidget (tBox);
        }
    }


  prefTab->addTab (czmilBox, tr ("CZMIL"));
  prefTab->setTabToolTip (6, tr ("Set CZMIL options"));
  prefTab->setTabWhatsThis (6, czmilText);


  vbox->addWidget (prefTab);


  setFields ();


  //  Connect this here so that it doesn't cause the IHO combo to go to Manual every time.

  connect (minZ, SIGNAL (valueChanged (double)), this, SLOT (slotMinZValueChanged (double)));


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (this);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  bRestoreDefaults = new QPushButton (tr ("Restore Defaults"), this);
  bRestoreDefaults->setToolTip (tr ("Restore all preferences to the default state"));
  bRestoreDefaults->setWhatsThis (restoreDefaultsText);
  connect (bRestoreDefaults, SIGNAL (clicked ()), this, SLOT (slotRestoreDefaults ()));
  actions->addWidget (bRestoreDefaults);

  QPushButton *applyButton = new QPushButton (tr ("OK"), this);
  applyButton->setToolTip (tr ("Accept changes and close dialog"));
  applyButton->setWhatsThis (applyPrefsText);
  connect (applyButton, SIGNAL (clicked ()), this, SLOT (slotApplyPrefs ()));
  actions->addWidget (applyButton);

  QPushButton *closeButton = new QPushButton (tr ("Cancel"), this);
  closeButton->setToolTip (tr ("Discard changes and close dialog"));
  closeButton->setWhatsThis (closePrefsText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotClosePrefs ()));
  actions->addWidget (closeButton);


  //  Set the current tab to the last used tab.

  prefTab->setCurrentIndex (options->pref_tab);
  connect (prefTab, SIGNAL (currentChanged (int)), this, SLOT (slotPrefTabChanged (int)));


  show ();
}



prefs::~prefs ()
{
}



void
prefs::slotPrefTabChanged (int tab)
{
  //  Force it to be the current tab in both the original and the copy, in case we change something.

  mod_options.pref_tab = options->pref_tab = tab;
}



void
prefs::slotContourColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.contour_color, this, tr ("pfmEdit Contour Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.contour_color = clr;

  setFields ();
}



void
prefs::slotTrackerColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.tracker_color, this, tr ("pfmEdit3D Tracker Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.tracker_color = clr;

  setFields ();
}



void
prefs::slotEditColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.edit_color, this, tr ("pfmEdit3D Edit Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.edit_color = clr;

  setFields ();
}



void
prefs::slotMarkerColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.marker_color, this, tr ("pfmEdit3D Marker Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.marker_color = clr;

  setFields ();
}



void
prefs::slotBackgroundColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.background_color, this, tr ("pfmEdit3D Background Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.background_color = clr;


  setFields ();
}



void
prefs::slotRefColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.ref_color[0], this, tr ("pfmEdit3D Reference Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.ref_color[0] = clr;

  setFields ();
}



void
prefs::slotFeatureColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.feature_color, this, tr ("pfmEdit3D Feature Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.feature_color = clr;

  setFields ();
}



void
prefs::slotFeatureInfoColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.feature_info_color, this, tr ("pfmEdit3D Feature Information Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.feature_info_color = clr;

  setFields ();
}



void
prefs::slotHighFeatureColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.feature_highlight_color, this, tr ("pfmEdit Highlighted Feature Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.feature_highlight_color = clr;

  setFields ();
}



void
prefs::slotVerFeatureColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.verified_feature_color, this, tr ("pfmEdit Verified Feature Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.verified_feature_color = clr;

  setFields ();
}



void
prefs::slotScaleColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.scale_color, this, tr ("pfmEdit3D Scale Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.scale_color = clr;

  setFields ();
}



void
prefs::slotScaledOffsetZColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.scaled_offset_z_color, this, tr ("pfmView Scaled/Offset Z Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.scaled_offset_z_color = clr;

  setFields ();
}



void
prefs::slotColorClicked (int id)
{
  QColor clr;

  QString name = tr ("pfmEdit3D Waveform/Marker %1 Color").arg (id);

  clr = QColorDialog::getColor (mod_options.waveColor[id], this, name, QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.waveColor[id] = clr;

  setFields ();
}



void
prefs::slotAncillaryPrograms ()
{
  if (ancillaryProgramD) ancillaryProgramD->close ();


  ancillaryProgramD = new QDialog (this, (Qt::WindowFlags) Qt::WA_DeleteOnClose);
  ancillaryProgramD->setWindowTitle (tr ("pfmEdit3D Ancillary Programs"));

  QVBoxLayout *vbox = new QVBoxLayout (ancillaryProgramD);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  QGroupBox *progBox = new QGroupBox (ancillaryProgramD);
  QHBoxLayout *progBoxLayout = new QHBoxLayout;
  progBox->setLayout (progBoxLayout);
  QVBoxLayout *progBoxLeftLayout = new QVBoxLayout;
  QVBoxLayout *progBoxRightLayout = new QVBoxLayout;
  progBoxLayout->addLayout (progBoxLeftLayout);
  progBoxLayout->addLayout (progBoxRightLayout);

  QGroupBox *programs[NUMPROGS];
  QHBoxLayout *programsLayout[NUMPROGS];

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      programs[i] = new QGroupBox (ancillaryProgramD);
      programsLayout[i] = new QHBoxLayout;
      programs[i]->setLayout (programsLayout[i]);
      
      program[i] = new QLineEdit (ancillaryProgramD);
      program[i]->setReadOnly (NVTrue);
      program[i]->setToolTip (options->description[i]);
      program[i]->setWhatsThis (programText);
      program[i]->setText (mod_options.name[i]);
      program[i]->setCursorPosition (0);
      programsLayout[i]->addWidget (program[i], 10);

      hotkey[i] = new QLineEdit (ancillaryProgramD);
      hotkey[i]->setToolTip (tr ("Hot key"));
      hotkey[i]->setWhatsThis (hotkeyText);
      hotkey[i]->setText (mod_options.hotkey[i]);
      hotkey[i]->setMinimumWidth (50);
      connect (hotkey[i], SIGNAL (textEdited (const QString &)), this, SLOT (slotApplyAncillaryChanges (const QString &)));
      programsLayout[i]->addWidget (hotkey[i], 1);

      actionkey[i] = new QLineEdit (ancillaryProgramD);
      actionkey[i]->setToolTip (tr ("Action key(s)"));
      actionkey[i]->setWhatsThis (actionkeyText);
      actionkey[i]->setText (mod_options.action[i]);
      actionkey[i]->setMinimumWidth (70);
      connect (actionkey[i], SIGNAL (textEdited (const QString &)), this, SLOT (slotApplyAncillaryChanges (const QString &)));
      programsLayout[i]->addWidget (actionkey[i], 1);


      if (!(i % 2))
        {
          progBoxLeftLayout->addWidget (programs[i]);
        }
      else
        {
          progBoxRightLayout->addWidget (programs[i]);
        }
    }


  vbox->addWidget (progBox);


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (ancillaryProgramD);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), ancillaryProgramD);
  closeButton->setToolTip (tr ("Close the ancillary program dialog"));
  closeButton->setWhatsThis (closeAncillaryProgramText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseAncillaryProgram ()));
  actions->addWidget (closeButton);


  //  Put the dialog in the middle of the screen.

  ancillaryProgramD->move (x () + width () / 2 - ancillaryProgramD->width () / 2, y () + height () / 2 - ancillaryProgramD->height () / 2);

  ancillaryProgramD->show ();
}



//!  Cheating again - any change will cause everything to update.  It only takes a second anyway.

void 
prefs::slotApplyAncillaryChanges (const QString &text __attribute__ ((unused)))
{
  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      mod_options.hotkey[i] = hotkey[i]->text ();
      mod_options.action[i] = actionkey[i]->text ();
    }
}



void 
prefs::slotCloseAncillaryProgram ()
{
  ancillaryProgramD->close ();
}



void
prefs::slotHelp ()
{
  QWhatsThis::enterWhatsThisMode ();
}



void
prefs::slotRestoreDefaults ()
{
  void set_defaults (MISC *misc, OPTIONS *options, uint8_t restore);


  set_defaults (misc, options, NVTrue);


  misc->abe_share->cint = 5.0;
  misc->abe_share->num_levels = 0;


  //  Default contour levels

  misc->abe_share->num_levels = 52;
  misc->abe_share->contour_levels[0] = -30;
  misc->abe_share->contour_levels[1] = -20;
  misc->abe_share->contour_levels[2] = -10;
  misc->abe_share->contour_levels[3] = -5;
  misc->abe_share->contour_levels[4] = -2;
  misc->abe_share->contour_levels[5] = 0;
  misc->abe_share->contour_levels[6] = 2;
  misc->abe_share->contour_levels[7] = 5;
  misc->abe_share->contour_levels[8] = 10;
  misc->abe_share->contour_levels[9] = 20;
  misc->abe_share->contour_levels[10] = 30;
  misc->abe_share->contour_levels[11] = 40;
  misc->abe_share->contour_levels[12] = 50;
  misc->abe_share->contour_levels[13] = 60;
  misc->abe_share->contour_levels[14] = 70;
  misc->abe_share->contour_levels[15] = 80;
  misc->abe_share->contour_levels[16] = 90;
  misc->abe_share->contour_levels[17] = 100;
  misc->abe_share->contour_levels[18] = 120;
  misc->abe_share->contour_levels[19] = 130;
  misc->abe_share->contour_levels[20] = 140;
  misc->abe_share->contour_levels[21] = 160;
  misc->abe_share->contour_levels[22] = 180;
  misc->abe_share->contour_levels[23] = 200;
  misc->abe_share->contour_levels[24] = 250;
  misc->abe_share->contour_levels[25] = 300;
  misc->abe_share->contour_levels[26] = 350;
  misc->abe_share->contour_levels[27] = 400;
  misc->abe_share->contour_levels[28] = 450;
  misc->abe_share->contour_levels[29] = 500;
  misc->abe_share->contour_levels[30] = 600;
  misc->abe_share->contour_levels[31] = 700;
  misc->abe_share->contour_levels[32] = 800;
  misc->abe_share->contour_levels[33] = 900;
  misc->abe_share->contour_levels[34] = 1000;
  misc->abe_share->contour_levels[35] = 1500;
  misc->abe_share->contour_levels[36] = 2000;
  misc->abe_share->contour_levels[37] = 2500;
  misc->abe_share->contour_levels[38] = 3000;
  misc->abe_share->contour_levels[39] = 3500;
  misc->abe_share->contour_levels[40] = 4000;
  misc->abe_share->contour_levels[41] = 4500;
  misc->abe_share->contour_levels[42] = 5000;
  misc->abe_share->contour_levels[43] = 5500;
  misc->abe_share->contour_levels[44] = 6000;
  misc->abe_share->contour_levels[45] = 6500;
  misc->abe_share->contour_levels[46] = 7000;
  misc->abe_share->contour_levels[47] = 7500;
  misc->abe_share->contour_levels[48] = 8000;
  misc->abe_share->contour_levels[49] = 8500;
  misc->abe_share->contour_levels[50] = 9000;
  misc->abe_share->contour_levels[51] = 10000;


  mod_options = *options;
  mod_share = *misc->abe_share;


  setFields ();


  *misc->abe_share = mod_share;
  *options = mod_options;


  //  Let the calling program (pfmView) know that we've changed some things that it uses.

  if (misc->abe_share != NULL)
    {
      misc->abe_share->settings_changed = NVTrue;
      misc->abe_share->position_form = options->position_form;
      misc->abe_share->z_factor = options->z_factor;
      misc->abe_share->z_offset = options->z_offset;


      //  Save the mosaic viewer program name and options.

      for (int32_t i = 0 ; i < NUMPROGS ; i++)
        {
          if (options->prog[i].contains ("mosaicView"))
            {
              strcpy (misc->abe_share->mosaic_prog, options->prog[i].toLatin1 ());
              strcpy (misc->abe_share->mosaic_actkey, options->action[i].toLatin1 ());
              strcpy (misc->abe_share->mosaic_hotkey, options->hotkey[i].toLatin1 ());
              break;
            }
        }


      misc->abe_share->settings_changed = NVTrue;
    }


  emit dataChangedSignal ();

  close ();
}



void
prefs::slotHspStateChanged (int state)
{
  if (state == Qt::Checked)
    {
      hmvBox->setEnabled (true);
      hmv->setEnabled (true);
    }
  else
    {
      hmvBox->setEnabled (false);
      hmv->setEnabled (false);
    }
}



void
prefs::slotPositionClicked (int id)
{
  mod_options.position_form = id;
}



void
prefs::slotCloseContours ()
{
  //  Get the values.

  QString string;
  int32_t j;

  j = 0;
  for (int32_t i = 0 ; i < MAX_CONTOUR_LEVELS ; i++)
    {
      string = contoursTable->item (i, 0)->text ();
      if (!string.isEmpty ())
        {
          mod_share.contour_levels[j] = string.toFloat ();
          j++;
        }
    }

  mod_share.num_levels = j + 1;

  contoursD->close ();
}



void
prefs::slotZFactorChanged (int index)
{
  //  If we changed the Z scaling factor and we have a Z offset set we need to divide the Z offsetby the old scale factor and then
  //  multiply the Z offset by the new scale factor otherwise things get decidedly screwy.

  mod_options.z_offset /= save_scale_factor;

  switch (index)
    {
    case 0:
      mod_options.z_factor = 1.0;
      break;

    case 1:
      mod_options.z_factor = 3.28084;
      break;

    case 2:
      mod_options.z_factor = 0.34080;
      break;

    case 3:
      mod_options.z_factor = 0.54681;
      break;

    case 4:
      mod_options.z_factor = 1.82880;
      break;

    case 5:
      mod_options.z_factor = 0.53333;
      break;
    }

  save_scale_factor = mod_options.z_factor;

  mod_options.z_offset *= save_scale_factor;

  offset->setValue (mod_options.z_offset);
}



void
prefs::slotOrientationClicked (int id)
{
  if (id)
    {
      mod_options.z_orientation = -1.0;
    }
  else
    {
      mod_options.z_orientation = 1.0;
    }
}



void
prefs::slotContourLocationClicked (int id)
{
  mod_options.contour_location = id;
}



void
prefs::slotContourLevelsClicked ()
{
  if (contoursD) slotCloseContours ();


  QString string;

  mod_share.cint = 0.0;
  string.sprintf ("%.2f", mod_share.cint);
  contourInt->setText (string);

  
  contoursD = new QDialog (this, (Qt::WindowFlags) Qt::WA_DeleteOnClose);
  contoursD->setWindowTitle (tr ("pfmEdit Contour Levels"));

  QVBoxLayout *vbox = new QVBoxLayout (contoursD);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  contoursTable = new QTableWidget (MAX_CONTOUR_LEVELS, 1, this);
  contoursTable->setWhatsThis (contoursTableText);
  contoursTable->setAlternatingRowColors (true);
  QTableWidgetItem *chItem = new QTableWidgetItem (tr ("Contour level"));
  contoursTable->setHorizontalHeaderItem (0, chItem);

  QTableWidgetItem *cntItem[MAX_CONTOUR_LEVELS];

  for (int32_t i = 0 ; i < MAX_CONTOUR_LEVELS ; i++)
    {
      if (i < mod_share.num_levels)
        {
          string.sprintf ("%f", mod_share.contour_levels[i]);
        }
      else
        {
          string.sprintf (" ");
        }
      cntItem[i] = new QTableWidgetItem (string); 
      contoursTable->setItem (i, 0, cntItem[i]);
    }
  contoursTable->resizeColumnsToContents ();


  vbox->addWidget (contoursTable, 1);

  int32_t w = qMin (800, contoursTable->columnWidth (0) + 60);
  contoursD->resize (w, 600);

  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (contoursD);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), contoursD);
  closeButton->setToolTip (tr ("Close the contour levels dialog"));
  closeButton->setWhatsThis (closeContoursText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseContours ()));
  actions->addWidget (closeButton);


  //  Put the dialog in the middle of the screen.

  contoursD->move (x () + width () / 2 - contoursD->width () / 2, y () + height () / 2 - contoursD->height () / 2);

  contoursD->show ();
}



void 
prefs::slotHotKeys ()
{
  if (hotKeyD) slotCloseHotKeys ();


  hotKeyD = new QDialog (this, (Qt::WindowFlags) Qt::WA_DeleteOnClose);
  hotKeyD->setWindowTitle (tr ("pfmEdit3D Hot Keys"));

  QVBoxLayout *vbox = new QVBoxLayout (hotKeyD);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  hotKeyTable = new QTableWidget (HOTKEYS, 2, this);
  hotKeyTable->setWhatsThis (hotKeyTableText);
  hotKeyTable->setAlternatingRowColors (true);
  QTableWidgetItem *bItemH = new QTableWidgetItem (tr ("Button"));
  hotKeyTable->setHorizontalHeaderItem (0, bItemH);
  QTableWidgetItem *kItemH = new QTableWidgetItem (tr ("Key"));
  hotKeyTable->setHorizontalHeaderItem (1, kItemH);

  QTableWidgetItem *bItem[HOTKEYS], *kItem[HOTKEYS];


  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      bItem[i] = new QTableWidgetItem (misc->buttonIcon[i], misc->buttonText[i]);
      bItem[i]->setFlags (Qt::ItemIsEnabled);

      hotKeyTable->setItem (i, 0, bItem[i]);
      kItem[i] = new QTableWidgetItem (mod_options.buttonAccel[i]);
      hotKeyTable->setItem (i, 1, kItem[i]);
    }
  hotKeyTable->resizeColumnsToContents ();
  hotKeyTable->resizeRowsToContents ();

  vbox->addWidget (hotKeyTable, 1);

  int32_t w = qMin (800, hotKeyTable->columnWidth (0) + hotKeyTable->columnWidth (1) + 40);
  int32_t h = qMin (600, hotKeyTable->rowHeight (0) * HOTKEYS + 75);
  hotKeyD->resize (w, h);


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (hotKeyD);
  bHelp->setIcon (QIcon (":/icons/contextHelp"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), hotKeyD);
  closeButton->setToolTip (tr ("Close the hot key dialog"));
  closeButton->setWhatsThis (closeHotKeyText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseHotKeys ()));
  actions->addWidget (closeButton);


  //  Put the dialog in the middle of the screen.

  hotKeyD->move (x () + width () / 2 - hotKeyD->width () / 2, y () + height () / 2 - hotKeyD->height () / 2);

  hotKeyD->show ();
}



void 
prefs::slotCloseHotKeys ()
{
  for (int32_t i = 0 ; i < HOTKEYS ; i++) mod_options.buttonAccel[i] = hotKeyTable->item (i, 1)->text ();

  hotKeyD->close ();
}



void 
prefs::slotEgmClicked ()
{
  double lat, lon;

  lat = misc->abe_share->edit_area.min_y + (misc->abe_share->edit_area.max_y - misc->abe_share->edit_area.min_y) / 2.0;
  lon = misc->abe_share->edit_area.min_x + (misc->abe_share->edit_area.max_x - misc->abe_share->edit_area.min_x) / 2.0;

  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();

  float datum_offset = get_egm08 (lat, lon);
  cleanup_egm08 ();

  qApp->restoreOverrideCursor ();

  if (datum_offset < 999999.0) 
    {
      offset->setValue ((double) datum_offset);
    }
  else
    {
      QMessageBox::warning (this, tr ("Get EGM08 datum offset"), tr ("Error retrieving EGM08 ellipsoid to geoid offset value"));
    }
}



void 
prefs::slotMinZValueChanged (double value __attribute__ ((unused)))
{
  //  We manually modified the Z value so we want to set the IHO combo box to "Manual".

  ihoOrder->setCurrentIndex (0);
}



void 
prefs::setFields ()
{
  QString string;


  //  Save the scale factor so we can play around with it and the offset value without messing up the input values.

  save_scale_factor = mod_options.z_factor;


  bGrp->button (mod_options.position_form)->setChecked (true);


  int32_t hue, sat, val;

  for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++)
    {
      mod_options.waveColor[i].getHsv (&hue, &sat, &val);
      if (val < 128)
        {
          bWavePalette[i].setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
          bWavePalette[i].setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
        }
      else
        {
          bWavePalette[i].setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
          bWavePalette[i].setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
        }
      bWavePalette[i].setColor (QPalette::Normal, QPalette::Button, mod_options.waveColor[i]);
      bWavePalette[i].setColor (QPalette::Inactive, QPalette::Button, mod_options.waveColor[i]);
      bWaveColor[i]->setPalette (bWavePalette[i]);
    }


  mod_options.contour_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bContourPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bContourPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bContourPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bContourPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bContourPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.contour_color);
  bContourPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.contour_color);
  bContourColor->setPalette (bContourPalette);


  mod_options.tracker_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bTrackerPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bTrackerPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bTrackerPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bTrackerPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bTrackerPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.tracker_color);
  bTrackerPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.tracker_color);
  bTrackerColor->setPalette (bTrackerPalette);


  mod_options.edit_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bEditPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bEditPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bEditPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bEditPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bEditPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.edit_color);
  bEditPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.edit_color);
  bEditColor->setPalette (bEditPalette);


  mod_options.marker_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bMarkerPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bMarkerPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bMarkerPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bMarkerPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bMarkerPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.marker_color);
  bMarkerPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.marker_color);
  bMarkerColor->setPalette (bMarkerPalette);


  mod_options.background_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bBackgroundPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bBackgroundPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bBackgroundPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bBackgroundPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bBackgroundPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.background_color);
  bBackgroundPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.background_color);
  bBackgroundColor->setPalette (bBackgroundPalette);


  mod_options.ref_color[0].getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bRefPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bRefPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bRefPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bRefPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bRefPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.ref_color[0]);
  bRefPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.ref_color[0]);
  bRefColor->setPalette (bRefPalette);


  mod_options.feature_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bFeaturePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bFeaturePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bFeaturePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bFeaturePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bFeaturePalette.setColor (QPalette::Normal, QPalette::Button, mod_options.feature_color);
  bFeaturePalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.feature_color);
  bFeatureColor->setPalette (bFeaturePalette);


  mod_options.feature_info_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bFeatureInfoPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bFeatureInfoPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bFeatureInfoPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bFeatureInfoPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bFeatureInfoPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.feature_info_color);
  bFeatureInfoPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.feature_info_color);
  bFeatureInfoColor->setPalette (bFeatureInfoPalette);


  mod_options.feature_highlight_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bHighFeaturePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bHighFeaturePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bHighFeaturePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bHighFeaturePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bHighFeaturePalette.setColor (QPalette::Normal, QPalette::Button, mod_options.feature_highlight_color);
  bHighFeaturePalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.feature_highlight_color);
  bHighFeatureColor->setPalette (bHighFeaturePalette);


  mod_options.verified_feature_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bVerFeaturePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bVerFeaturePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bVerFeaturePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bVerFeaturePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bVerFeaturePalette.setColor (QPalette::Normal, QPalette::Button, mod_options.verified_feature_color);
  bVerFeaturePalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.verified_feature_color);
  bVerFeatureColor->setPalette (bVerFeaturePalette);


  mod_options.scale_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bScalePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bScalePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bScalePalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bScalePalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bScalePalette.setColor (QPalette::Normal, QPalette::Button, mod_options.scale_color);
  bScalePalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.scale_color);
  bScaleColor->setPalette (bScalePalette);


  mod_options.scaled_offset_z_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bScaledOffsetZPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bScaledOffsetZPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bScaledOffsetZPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bScaledOffsetZPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bScaledOffsetZPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.scaled_offset_z_color);
  bScaledOffsetZPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.scaled_offset_z_color);
  bScaledOffsetZColor->setPalette (bScaledOffsetZPalette);


  noMaskedCheck->setChecked (mod_options.no_color_scale_masked);
  graySliceCheck->setChecked (mod_options.slice_gray);
  whiteInvalidCheck->setChecked (mod_options.invalid_edit_color);

  if (mod_options.z_orientation > 0.0)
    {
      depthCheck->setChecked (true);
    }
  else
    {
      elevationCheck->setChecked (true);
    }

  if (mod_options.contour_location)
    {
      cloudCheck->setChecked (true);
    }
  else
    {
      topCheck->setChecked (true);
    }

  string.sprintf ("%.2f", mod_share.cint);
  contourInt->setText (string);

  contourSm->setValue (mod_options.smoothing_factor);


  if (fabs (mod_options.z_factor - 1.0) < 0.0000001)
    {
      ZFactor->setCurrentIndex (0);
    }
  else if (fabs (mod_options.z_factor - 3.28084) < 0.0000001)
    {
      ZFactor->setCurrentIndex (1);
    }
  else if (fabs (mod_options.z_factor - 0.34080) < 0.0000001)
    {
      ZFactor->setCurrentIndex (2);
    }
  else if (fabs (mod_options.z_factor - 0.54681) < 0.0000001)
    {
      ZFactor->setCurrentIndex (3);
    }
  else if (fabs (mod_options.z_factor - 1.82880) < 0.0000001)
    {
      ZFactor->setCurrentIndex (4);
    }
  else if (fabs (mod_options.z_factor - 0.53333) < 0.0000001)
    {
      ZFactor->setCurrentIndex (5);
    }

  offset->setValue (mod_options.z_offset);

  string.sprintf ("%d", mod_options.contour_width);
  Width->lineEdit ()->setText (string);

  pointSize->setValue (mod_options.point_size);

  sparse->setValue (mod_options.sparse_limit);

  minZ->setValue (mod_options.min_window_size);

  ihoOrder->setCurrentIndex (mod_options.iho_min_window);

  dFilter->setChecked (mod_options.deep_filter_only);

  featureRadius->setValue (mod_options.feature_radius);

  hpThreads->setCurrentIndex (NINT (log10 ((double) mod_options.num_hp_threads) / log10 (4.0)));

  aFilter->setChecked (mod_options.auto_filter_mask);

  switch (mod_options.main_button_icon_size)
    {
    case 16:
      iconSize->setCurrentIndex (0);
      break;

    case 20:
      iconSize->setCurrentIndex (1);
      break;

    case 24:
    default:
      iconSize->setCurrentIndex (2);
      break;

    case 28:
      iconSize->setCurrentIndex (3);
      break;

    case 32:
      iconSize->setCurrentIndex (4);
      break;
    }

      
  zoomPercent->setValue (mod_options.zoom_percent);
  rot->setValue (mod_options.rotation_increment);
  fSize->setValue (mod_options.feature_size);
  overlap->setValue (mod_options.overlap_percent);
  undo->setValue (mod_options.undo_levels);
  kill->setChecked (mod_options.kill_and_respawn);
  tip->setChecked (mod_options.highlight_tip);
  hsp->setChecked (mod_options.hide_persistent);
  hmv->setChecked (mod_options.hide_visible);
  scale->setChecked (mod_options.draw_scale);
  czmilWarnings->setChecked (mod_options.czmil_suppress_warnings);
  autoScale->setChecked (mod_options.auto_scale);
  for (int32_t i = 0 ; i < 9 ; i++) threshold[i]->setValue (mod_options.czmil_probability[i]);
  for (int32_t i = 0 ; i < 2 ; i++) czmilMask[i]->setChecked (mod_options.czmil_reprocess_flags[i]);
}



void
prefs::slotApplyPrefs ()
{
  if (mod_options.position_form != options->position_form) dataChanged = NVTrue;


  if (mod_options.z_orientation != options->z_orientation) dataChanged = NVTrue;


  if (mod_options.contour_location != options->contour_location) dataChanged = NVTrue;

  QString tmp = contourInt->text ();
  if (!tmp.isEmpty ()) mod_share.cint = tmp.toFloat ();
  if (mod_share.cint != misc->abe_share->cint) dataChanged = NVTrue;

  tmp = contourSm->text ();
  if (!tmp.isEmpty ()) mod_options.smoothing_factor = tmp.toInt ();
  if (mod_options.smoothing_factor != options->smoothing_factor) dataChanged = NVTrue;


  //  We don't have to retrieve this since we have slotZFactorChanged.

  if (mod_options.z_factor != options->z_factor) dataChanged = NVTrue;


  mod_options.z_offset = offset->value ();
  if (mod_options.z_offset != options->z_offset) dataChanged = NVTrue;


  mod_options.point_size = pointSize->value ();
  if (mod_options.point_size != options->point_size) dataChanged = NVTrue;

  mod_options.sparse_limit = sparse->value ();
  if (mod_options.sparse_limit != options->sparse_limit) dataChanged = NVTrue;

  tmp = Width->currentText ();
  if (!tmp.isEmpty ()) mod_options.contour_width = tmp.toInt ();
  if (mod_options.contour_width != options->contour_width) dataChanged = NVTrue;


  mod_options.min_window_size = minZ->value ();
  if (mod_options.min_window_size != options->min_window_size) dataChanged = NVTrue;

  mod_options.iho_min_window = ihoOrder->currentIndex ();
  if (mod_options.iho_min_window != options->iho_min_window) dataChanged = NVTrue;


  //  We don't need to say that the data changed if we just mod the filter options.

  mod_options.deep_filter_only = dFilter->isChecked ();
  if (mod_options.deep_filter_only != options->deep_filter_only) options->deep_filter_only = mod_options.deep_filter_only;

  mod_options.num_hp_threads = NINT (pow (4.0, (double) hpThreads->currentIndex ()));
  if (mod_options.num_hp_threads != options->num_hp_threads) dataChanged = NVTrue;

  mod_options.feature_radius = featureRadius->value ();
  if (mod_options.feature_radius != options->feature_radius) options->feature_radius = mod_options.feature_radius;

  mod_options.auto_filter_mask = aFilter->isChecked ();
  if (mod_options.auto_filter_mask != options->auto_filter_mask) options->auto_filter_mask = mod_options.auto_filter_mask;

  switch (iconSize->currentIndex ())
    {
    case 0:
      mod_options.main_button_icon_size = 16;
      break;

    case 1:
      mod_options.main_button_icon_size = 20;
      break;

    case 2:
      mod_options.main_button_icon_size = 24;
      break;

    case 3:
      mod_options.main_button_icon_size = 28;
      break;

    case 4:
      mod_options.main_button_icon_size = 32;
      break;
    }
  if (mod_options.main_button_icon_size != options->main_button_icon_size) dataChanged = NVTrue;

  mod_options.rotation_increment = rot->value ();
  if (mod_options.rotation_increment != options->rotation_increment) dataChanged = NVTrue;

  mod_options.overlap_percent = overlap->value ();
  if (mod_options.overlap_percent != options->overlap_percent) dataChanged = NVTrue;

  mod_options.feature_size = fSize->value ();
  if (mod_options.feature_size != options->feature_size) dataChanged = NVTrue;

  mod_options.zoom_percent = zoomPercent->value ();
  if (mod_options.zoom_percent != options->zoom_percent) dataChanged = NVTrue;

  mod_options.undo_levels = undo->value ();
  if (mod_options.undo_levels != options->undo_levels && resize_undo (misc, options, mod_options.undo_levels)) dataChanged = NVTrue;

  mod_options.kill_and_respawn = kill->isChecked ();
  if (mod_options.kill_and_respawn != options->kill_and_respawn) dataChanged = NVTrue;

  mod_options.highlight_tip = tip->isChecked ();
  if (mod_options.highlight_tip != options->highlight_tip) dataChanged = NVTrue;

  mod_options.hide_persistent = hsp->isChecked ();
  if (mod_options.hide_persistent != options->hide_persistent) dataChanged = NVTrue;

  mod_options.hide_visible = hmv->isChecked ();
  if (mod_options.hide_visible != options->hide_visible) dataChanged = NVTrue;

  mod_options.draw_scale = scale->isChecked ();
  if (mod_options.draw_scale != options->draw_scale) dataChanged = NVTrue;

  mod_options.czmil_suppress_warnings = czmilWarnings->isChecked ();
  if (mod_options.czmil_suppress_warnings != options->czmil_suppress_warnings) dataChanged = NVTrue;

  mod_options.auto_scale = autoScale->isChecked ();
  if (mod_options.auto_scale != options->auto_scale) dataChanged = NVTrue;

  if (mod_options.contour_color != options->contour_color) dataChanged = NVTrue;
  if (mod_options.tracker_color != options->tracker_color) dataChanged = NVTrue;
  if (mod_options.edit_color != options->edit_color) dataChanged = NVTrue;
  if (mod_options.marker_color != options->marker_color) dataChanged = NVTrue;
  if (mod_options.background_color != options->background_color) dataChanged = NVTrue;
  if (mod_options.ref_color[0] != options->ref_color[0]) dataChanged = NVTrue;
  if (mod_options.feature_color != options->feature_color) dataChanged = NVTrue;
  if (mod_options.feature_info_color != options->feature_info_color) dataChanged = NVTrue;
  if (mod_options.verified_feature_color != options->verified_feature_color) dataChanged = NVTrue;
  if (mod_options.scale_color != options->scale_color) dataChanged = NVTrue;
  if (mod_options.scaled_offset_z_color != options->scaled_offset_z_color) dataChanged = NVTrue;
  mod_options.no_color_scale_masked = noMaskedCheck->isChecked ();
  if (mod_options.no_color_scale_masked != options->no_color_scale_masked) dataChanged = NVTrue;
  mod_options.slice_gray = graySliceCheck->isChecked ();
  if (mod_options.slice_gray != options->slice_gray) dataChanged = NVTrue;
  mod_options.invalid_edit_color = whiteInvalidCheck->isChecked ();
  if (mod_options.invalid_edit_color != options->invalid_edit_color) dataChanged = NVTrue;


  for (int32_t i = 0 ; i < 9 ; i++)
    {
      mod_options.czmil_probability[i] = threshold[i]->value ();
      if (mod_options.czmil_probability[i] != options->czmil_probability[i]) dataChanged = NVTrue;
    }

  for (int32_t i = 0 ; i < 2 ; i++)
    {
      if (czmilMask[i]->isChecked ())
        {
          mod_options.czmil_reprocess_flags[i] = NVTrue;
        }
      else
        {
          mod_options.czmil_reprocess_flags[i] = NVFalse;
        }
      if (mod_options.czmil_reprocess_flags[i] != options->czmil_reprocess_flags[i]) dataChanged = NVTrue;
    }


  for (int32_t i = 0 ; i < MAX_STACK_POINTS ; i++)
    {
      if (mod_options.waveColor[i] != options->waveColor[i])
        {
          mod_share.mwShare.multiColors[i].r = mod_options.waveColor[i].red ();
          mod_share.mwShare.multiColors[i].g = mod_options.waveColor[i].green ();
          mod_share.mwShare.multiColors[i].b = mod_options.waveColor[i].blue ();
          mod_share.mwShare.multiColors[i].a = mod_options.waveColor[i].alpha ();
          dataChanged = NVTrue;
        }
    }


  //  Make sure we have no hotkey duplications.  First, the buttons and programs against hard-wired keys.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      if (mod_options.buttonAccel[i].toUpper () == "ESC")
        {
          QMessageBox::warning (0, tr ("pfmEdit3D preferences"), tr ("Esc is not allowed as a hot key.  It is reserved for disabling slice mode."));

          mod_options.buttonAccel[i] = options->buttonAccel[i];
          dataChanged = NVFalse;

          hotKeyD->close ();
          hotKeyD = NULL;

          return;
        }

      for (int32_t j = 0 ; j < HARD_KEYS ; j++)
        {
          if (mod_options.buttonAccel[i].toUpper () == misc->hard_key[j].toUpper ())
            {
              QMessageBox::warning (0, tr ("pfmEdit3D preferences"),
                                    misc->hard_key[j] + tr (" is not allowed as a hot key.  It is reserved for a color-by option."));

              mod_options.buttonAccel[i] = options->buttonAccel[i];
              dataChanged = NVFalse;

              hotKeyD->close ();
              hotKeyD = NULL;

              return;
            }
        }
    }

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      if (mod_options.hotkey[i].toUpper () == "ESC")
        {
          QMessageBox::warning (0, tr ("pfmEdit3D preferences"), tr ("Esc is not allowed as a hot key.  It is reserved for disabling slice mode."));

          mod_options.hotkey[i] = options->hotkey[i];
          dataChanged = NVFalse;

          ancillaryProgramD->close ();
          ancillaryProgramD = NULL;

          return;
        }

      for (int32_t j = 0 ; j < HARD_KEYS ; j++)
        {
          if (mod_options.hotkey[i].toUpper () == misc->hard_key[j].toUpper ())
            {
              QMessageBox::warning (0, tr ("pfmEdit3D preferences"),
                                    misc->hard_key[j] + tr (" is not allowed as a hot key.  It is reserved for a color-by option."));

              mod_options.buttonAccel[i] = options->buttonAccel[i];
              dataChanged = NVFalse;

              hotKeyD->close ();
              hotKeyD = NULL;

              return;
            }
        }
      if (mod_options.buttonAccel[i].toUpper () == "CTRL+D")
        {
          QMessageBox::warning (0, tr ("pfmEdit3D preferences"),
                                tr ("Ctrl+d is not allowed as a hot key.  It is reserved for setting color by depth/elevation."));

          mod_options.buttonAccel[i] = options->buttonAccel[i];
          dataChanged = NVFalse;

          hotKeyD->close ();
          hotKeyD = NULL;

          return;
        }

      if (mod_options.buttonAccel[i].toUpper () == "CTRL+L")
        {
          QMessageBox::warning (0, tr ("pfmEdit3D preferences"),
                                tr ("Ctrl+l is not allowed as a hot key.  It is reserved for setting color by line number."));

          mod_options.buttonAccel[i] = options->buttonAccel[i];
          dataChanged = NVFalse;

          hotKeyD->close ();
          hotKeyD = NULL;

          return;
        }
    }


  //  Next, the buttons against the buttons.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      for (int32_t j = i + 1 ; j < HOTKEYS ; j++)
        {
          if (mod_options.buttonAccel[i].toUpper () == mod_options.buttonAccel[j].toUpper ())
            {
              QMessageBox::warning (0, tr ("pfmEdit3D preferences"),
                                    tr ("Hot key for <b>%1</b> button conflicts with hotkey for <b>%2</b> button!<br><br>").arg 
                                    (misc->buttonText[i]).arg (misc->buttonText[j]) +
                                    tr ("Resetting button hotkeys for these two buttons."));

              mod_options.buttonAccel[i] = options->buttonAccel[i];
              mod_options.buttonAccel[j] = options->buttonAccel[j];
              dataChanged = NVFalse;

              hotKeyD->close ();
              hotKeyD = NULL;

              return;
            }
        }
    }


  //  Next, the programs against the programs.

  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      for (int32_t j = i + 1 ; j < NUMPROGS ; j++)
        {
          if (mod_options.hotkey[i].toUpper () == mod_options.hotkey[j].toUpper ())
            {
              QMessageBox::warning (0, tr ("pfmEdit3D preferences"),
                                    tr ("Hot key for <b>%1</b> program conflicts with hotkey for <b>%2</b> program!<br><br>").arg
                                    (options->name[i]).arg (options->name[j]) +
                                    tr ("Resetting program hotkeys for these two programs."));

              mod_options.hotkey[i] = options->hotkey[i];
              mod_options.hotkey[j] = options->hotkey[j];
              dataChanged = NVFalse;

              ancillaryProgramD->close ();
              ancillaryProgramD = NULL;

              return;
            }
        }
    }


  //  Finally, the buttons against the programs.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      for (int32_t j = 0 ; j < NUMPROGS ; j++)
        {
          if (mod_options.buttonAccel[i].toUpper () == mod_options.hotkey[j].toUpper ())
            {
              QMessageBox::warning (0, tr ("pfmEdit3D preferences"),
                                    tr ("Hot key for <b>%1</b> button conflicts with hotkey for <b>%2</b> program!<br><br>").arg
                                    (misc->buttonText[i]).arg (options->name[j]) +
                                    tr ("Resetting button and program hotkeys."));

              mod_options.buttonAccel[i] = options->buttonAccel[i];
              mod_options.hotkey[j] = options->hotkey[j];
              dataChanged = NVFalse;

              hotKeyD->close ();
              hotKeyD = NULL;
              ancillaryProgramD->close ();
              ancillaryProgramD = NULL;

              return;
            }
        }
    }


  //  Now check for changes to the hotkeys.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      if (mod_options.buttonAccel[i] != options->buttonAccel[i])
        {
          dataChanged = NVTrue;
          emit hotKeyChangedSignal (i);
          break;
        }
    }


  for (int32_t i = 0 ; i < NUMPROGS ; i++)
    {
      if (mod_options.hotkey[i] != options->hotkey[i] ||
          mod_options.action[i] != options->action[i])
        {
          dataChanged = NVTrue;
          break;
        }

      for (int32_t j = 0 ; j < PFM_DATA_TYPES ; j++)
        {
          if (mod_options.data_type[i][j] != options->data_type[i][j])
            {
              dataChanged = NVTrue;
              break;
            }
        }

      if (dataChanged) break;
    }


  if (mod_share.num_levels != misc->abe_share->num_levels) dataChanged = NVTrue;


  for (int32_t i = 0 ; i < MAX_CONTOUR_LEVELS ; i++)
    {
      if (mod_share.contour_levels[i] != misc->abe_share->contour_levels[i])
        {
          dataChanged = NVTrue;
          break;
        }
    }


  setFields ();


  hide ();


  if (dataChanged)
    {
      *misc->abe_share = mod_share;
      *options = mod_options;


      //  Let the calling program (pfmView) know that we've changed some things that it uses.

      if (misc->abe_share != NULL)
        {
          misc->abe_share->settings_changed = NVTrue;
          misc->abe_share->position_form = options->position_form;
          misc->abe_share->z_factor = options->z_factor;
          misc->abe_share->z_offset = options->z_offset;


          //  Save the mosaic viewer program name and options.

          for (int32_t i = 0 ; i < NUMPROGS ; i++)
            {
              if (options->prog[i].contains ("mosaicView"))
                {
                  strcpy (misc->abe_share->mosaic_prog, options->prog[i].toLatin1 ());
                  strcpy (misc->abe_share->mosaic_actkey, options->action[i].toLatin1 ());
                  strcpy (misc->abe_share->mosaic_hotkey, options->hotkey[i].toLatin1 ());

                  break;
                }
            }

          misc->abe_share->settings_changed = NVTrue;
        }


      emit dataChangedSignal ();
    }

  close ();
}



void
prefs::slotClosePrefs ()
{
  close ();
}
