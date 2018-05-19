source "util.tk"

proc createGUI { parent } {
  set menubar $parent.menubar

  frame $menubar -relief raised -borderwidth 2

  menubutton $menubar.file -text "File" -menu .menubar.file.menu
  menubutton $menubar.step -text "Step" -menu .menubar.step.menu
  menubutton $menubar.help -text "Help" -menu .menubar.help.menu

  pack $menubar.file -side left
  pack $menubar.step -side left
  pack $menubar.help -side right

  menu $menubar.file.menu
  menu $menubar.step.menu
  menu $menubar.help.menu

  $menubar.file.menu add command -label "Exit" -command { exit }

  $menubar.step.menu add checkbutton -label "Cull" \
    -command { cullStep }
  $menubar.step.menu add checkbutton -label "Backface Removal" \
    -command { backfaceRemovalStep }
  $menubar.step.menu add checkbutton -label "Light" \
    -command { lightStep }
  $menubar.step.menu add checkbutton -label "Sort" \
    -command { sortStep } -variable sort_step
  $menubar.step.menu add checkbutton -label "Clip" \
    -command { clipStep }

  $menubar.help.menu add command -label "Help" -command { help }

  pack $menubar -side top -fill x

  #----

  set    str "R0lGODlhEAAQAPAAAAAAAAAA/yH5BAEAAAAALAAAAAAQABAAAAIrhB+pe5aN"
  append str "QpMSUuRsBpVP3HlbtpRaWIUeOpKfCE4l09Vv+sLXLUOrNVNACgA7"

  set wireframe_image [image create photo -data $str]

  set    str "R0lGODlhEAAQAPEAAAAAAAAA/////wAAACH5BAEAAAAALAAAAAAQABAAAAIw"
  append str "hB+pexYCI0ztyfgCung6PmGAxZGCtqQXCprn2K5wKbuhxNLivO487tnQNJUU"
  append str "o1EAADs="

  set solid_image [image create photo -data $str]

  set    str "R0lGODlhEAAQAPAAAAAAAAAAACH5BAEAAAAALAAAAAAQABAAAAInhI9pEYq8"
  append str "oGMttnrloZhO6SmcVTnmiZKqCGpPOL7XAoS1K0congIFADs="

  set zoom_image [image create photo -data $str]

  set    str "R0lGODlhEAAQAPAAAAAAAAAAACH5BAEAAAAALAAAAAAQABAAAAIphI9pwaHt"
  append str "QFOLVZvgkvjKjE0UV43miSJhRIni4bxcfLGw24GRptspUAAAOw=="

  set unzoom_image [image create photo -data $str]

  set    str "R0lGODlhEAAQAPIAAAAAAAAAAD8/PwAA//8AAP///z///wAAACH5BAEAAAAA"
  append str "LAAAAAAQABAAAANICLo2saA9GB0bgiq5se5Pk31VIBEEKWEpkBpwLJox0Qx4"
  append str "rhM4HwiCgFCAEaAIxgnI4lK0QBzns5KJakSgD7YzonguXYgyq0kAADs="

  set rotatex_image [image create photo -data $str]

  set    str "R0lGODlhEAAQAPIAAAAAAAAAAD8/PwAA//8AAP///z///wAAACH5BAEAAAAA"
  append str "LAAAAAAQABAAAANGCLo2saA9GB0bgiq5se5Pk31VwJGlBxlsK5puM8x0bQeC"
  append str "EOwCpvO7lcOCkjQIRQzBQGh+RMgjACnMRD3UhWradFImFzAkAQA7"

  set rotatey_image [image create photo -data $str]

  set    str "R0lGODlhEAAQAPIAAAAAAAAAAD8/PwAA//8AAP///z///wAAACH5BAEAAAAA"
  append str "LAAAAAAQABAAAANFCLo2oYS0t9SELVbrLBHadjUgQGzAdJlo6gyCaMw0HND1"
  append str "oO98HwiCgFAACwqPlUmnxbktWyQXBRWVQmEMLErLiaGmzVYCADs="

  set rotatez_image [image create photo -data $str]

  #----

  set toolbar $parent.toolbar

  frame $toolbar -relief raised -borderwidth 2

  button $toolbar.wireframe -image $wireframe_image -command WireframeMode
  button $toolbar.solid     -image $solid_image     -command SolidMode
  button $toolbar.zoom      -image $zoom_image      -command ZoomIn
  button $toolbar.unzoom    -image $unzoom_image    -command ZoomOut
  button $toolbar.rotatex   -image $rotatex_image   -command RotateX
  button $toolbar.rotatey   -image $rotatey_image   -command RotateY
  button $toolbar.rotatez   -image $rotatez_image   -command RotateZ

  pack $toolbar.wireframe -side left
  pack $toolbar.solid     -side left
  pack $toolbar.zoom      -side left
  pack $toolbar.unzoom    -side left
  pack $toolbar.rotatex   -side left
  pack $toolbar.rotatey   -side left
  pack $toolbar.rotatez   -side left

  pack $toolbar -side top -fill x

  #----

  frame .frame

  #----

  global renderer

  set renderer .frame.renderer

  CTclRenderer3D $renderer -width 800 -height 600

  pack .frame.renderer -side top -anchor w -fill both -expand true

  #----

  pack .frame -side left -fill both -expand true

  #----

  after 100 updateTimer
}

proc WireframeMode { } {
  global renderer

  $renderer setmode wireframe
}

proc SolidMode { } {
  global renderer

  $renderer setmode solid
}

proc ZoomIn { } {
  global renderer

  $renderer zoom in
}

proc ZoomOut { } {
  global renderer

  $renderer zoom out
}

proc RotateX { } {
  global renderer

  $renderer rotate x
}

proc RotateY { } {
  global renderer

  $renderer rotate y
}

proc RotateZ { } {
  global renderer

  $renderer rotate z
}

proc updateTimer { } {
  global renderer

  $renderer update

  after 100 updateTimer
}

proc cullStep { } {
}

proc backfaceRemovalStep { } {
}

proc lightStep { } {
}

proc sortStep { } {
  global renderer sort_step

  if {$sort_step} {
    $renderer step add sort
  } \
  else {
    $renderer step remove sort
  }
}

proc clipStep { } {
}

createGUI ""
