#!/usr/bin/env tclsh

package require Tk

# Process to report state of all variables

proc reportState { } {
   global   exitFlag
   global   sonarEna
   global   lineEna 
   global   rtcEna
   global   accelEna
   global   motorType
   global   Kp
   global   Ki
   global   Kd
   global   samplePeriod
   global   wheelDiam
   global   turnRad
   global   ticsPerRev
   global   M1Ena
   global   M2Ena
   global   M3Ena
   global   M4Ena
   global   PWMresMode
   global   Kp_sp
   global   Ki_sp
   global   Kd_sp
   global   max_delta
   global   velPIDscale 
   global   spPIDscale

   puts -nonewline "$exitFlag:"
   puts -nonewline "$sonarEna:"
   puts -nonewline "$lineEna:"
   puts -nonewline "$rtcEna:"
   puts -nonewline "$accelEna:"
   puts -nonewline "$motorType:"
   puts -nonewline "$Kp:"
   puts -nonewline "$Ki:"
   puts -nonewline "$Kd:"
   puts -nonewline "$samplePeriod:"
   puts -nonewline "$wheelDiam:"
   puts -nonewline "$turnRad:"
   puts -nonewline "$ticsPerRev:"
   puts -nonewline "$M1Ena:"
   puts -nonewline "$M2Ena:"
   puts -nonewline "$M3Ena:"
   puts -nonewline "$M4Ena:"
   puts -nonewline "$PWMresMode:"  
   puts -nonewline "$Kp_sp:"   
   puts -nonewline "$Ki_sp:"  
   puts -nonewline "$Kd_sp:"  
   puts -nonewline "$max_delta:"  
   puts -nonewline "$velPIDscale:"  
   puts -nonewline "$spPIDscale"   
   puts ""
}

#
# Procedure to read in the robot.config file
#

proc readConfig { } {
  global data

  set fp [open "../robot.config" r]
  set file_data [read $fp]
  set data [split $file_data ":"]
  close $fp
}

# Procedure to toggle color of "apply" button

proc buttonColor {{x 0}} {
  global applyPressed  

  if {$applyPressed} {
#     puts "Apply pressed"
     set  applyPressed 0
     .b_run config -bg green -activebackground green 
  } else {
     .b_run config -bg red -activebackground red 
  }
}

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# START OF MAIN TCL ROUTINE
#
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

set data {}
readConfig

#
# Set defaults
# 

set exitFlag     [lindex $data 0]
set sonarEna     [lindex $data 1]
set lineEna      [lindex $data 2]
set rtcEna       [lindex $data 3]
set accelEna     [lindex $data 4]
set motorType    [lindex $data 5]
set Kp           [lindex $data 6]
set Ki           [lindex $data 7]
set Kd           [lindex $data 8]
set samplePeriod [lindex $data 9]
set wheelDiam    [lindex $data 10]
set turnRad      [lindex $data 11]
set ticsPerRev   [lindex $data 12]
set M1Ena        [lindex $data 13]
set M2Ena        [lindex $data 14]
set M3Ena        [lindex $data 15]
set M4Ena        [lindex $data 16]
set PWMresMode   [lindex $data 17]
set Kp_sp        [lindex $data 18]
set Ki_sp        [lindex $data 19]
set Kd_sp        [lindex $data 20]
set max_delta    [lindex $data 21]
set velPIDscale  [lindex $data 22]
set spPIDscale   [lindex $data 23]

set   cnt    0
set   cmd    buttonColor 

# Flags

set   applyPressed    0
set   exitFlag        0

# Chose a font

set    font   {Mono 12}
set    bw      5

# Give the window a title

wm title . "SIUE IEEE Robot Project"

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# Describe the header area
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

frame .headerarea -borderwidth 5 -relief groove
pack .headerarea -fill x

label .l_title   -text  "SIUE IEEE Robot" -font "Mono 36"
pack  .l_title -in .headerarea

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# Describe the button area
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

# ^^^^^^^^^^^^^^^^  Features Enabled ^^^^^^^^^^^^^^^^^^^^^^^

frame .buttonarea -borderwidth $bw -height 500
pack .buttonarea -fill x 

label .l_feature_ena  -text "Feature enabled:  " -font $font


checkbutton .b_sonar   -text "   Sonars    " -variable sonarEna -justify right -background yellow \
         -font $font -fg black -relief raised -height 1 \
         -command $cmd

checkbutton .b_line    -text " Line Follow " -variable lineEna -justify right -background yellow\
         -font $font -fg black -relief raised -height 1 \
         -command $cmd

checkbutton .b_rtc     -text "     RTC     " -variable rtcEna -justify right -background yellow \
         -font $font -fg black -relief raised -height 1 \
         -command $cmd

checkbutton .b_accel   -text " Acclerometer " -variable accelEna -justify right -background yellow \
        -font $font -fg black -relief raised -height 1 \
        -command $cmd

grid config .l_feature_ena   -in .buttonarea  -column 0 -row 1 -sticky "w"
grid config .b_sonar         -in .buttonarea  -column 1 -row 1 -sticky "e"
grid config .b_line          -in .buttonarea  -column 2 -row 1 -sticky "e"
grid config .b_rtc           -in .buttonarea  -column 3 -row 1 -sticky "e"
grid config .b_accel         -in .buttonarea  -column 4 -row 1 -sticky "e"

#  ^^^^^^^^^^^^^^^^^^^^^^^^^ Motor Type (DC or Stepper) ^^^^^^^^^^^^^^^^^^

radiobutton .b_dc -variable motorType \
          -text "      DC     "  -value 1 -justify right -background white \
          -font $font -fg black -relief raised -height 1 \
          -command $cmd

radiobutton .b_stepper -variable motorType \
          -text "    Servo    " -value 2 -justify right -background white \
          -font $font -fg black -relief raised -height 1 \
          -command $cmd

label .l_motor_type   -text "Select motor type:  " -font $font

grid config .l_motor_type   -in .buttonarea  -column 0 -row 2 -sticky "w"
grid config .b_dc           -in .buttonarea  -column 1 -row 2 -sticky "e"
grid config .b_stepper      -in .buttonarea  -column 2 -row 2 -sticky "e"


#  ^^^^^^^^^^^^^^^^^^^^^^^^^ PWM resolution ^^^^^^^^^^^^^^^^^^

label .l_PWMres_mode   -text  "Select PWM resolution:  " -font $font

radiobutton .b_8bit -variable PWMresMode \
          -text "   8 bits    "  -value 1 -justify right -background violet \
          -font $font -fg black -relief raised -height 1 \
          -command $cmd

radiobutton .b_10bit -variable PWMresMode \
          -text "  10 bits    " -value 2 -justify right -background violet \
          -font $font -fg black -relief raised -height 1 \
          -command $cmd

radiobutton .b_12bit -variable PWMresMode \
          -text "  12 bits    " -value 3 -justify right -background violet \
          -font $font -fg black -relief raised -height 1 \
          -command $cmd

grid config .l_PWMres_mode  -in .buttonarea  -column 0 -row 3 -sticky "w"
grid config .b_8bit         -in .buttonarea  -column 1 -row 3 -sticky "e"
grid config .b_10bit        -in .buttonarea  -column 2 -row 3 -sticky "e"
grid config .b_12bit        -in .buttonarea  -column 3 -row 3 -sticky "e"


# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Motors Enabled ^^^^^^^^^^^^^^^^^^^^^^

label .l_motor_ena  -text "Motors enabled:  " -font $font

checkbutton .b_M1   -text  "      M1      " -variable M1Ena -justify right -background yellow \
         -font $font -fg black -relief raised -height 1 \
         -command $cmd

checkbutton .b_M2    -text "     M2      " -variable M2Ena -justify right -background yellow \
         -font $font -fg black -relief raised -height 1 \
         -command $cmd

checkbutton .b_M3     -text "    M3      " -variable M3Ena -justify right -background yellow \
         -font $font -fg black -relief raised -height 1 \
         -command $cmd

checkbutton .b_M4     -text "    M4      " -variable M4Ena -justify right -background yellow \
        -font $font -fg black -relief raised -height 1 \
        -command $cmd

grid config .l_motor_ena     -in .buttonarea  -column 0 -row 4 -sticky "w"
grid config .b_M1            -in .buttonarea  -column 1 -row 4 -sticky "e"
grid config .b_M2            -in .buttonarea  -column 2 -row 4 -sticky "e"
grid config .b_M3            -in .buttonarea  -column 3 -row 4 -sticky "e"
grid config .b_M4            -in .buttonarea  -column 4 -row 4 -sticky "e"

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# Describe the velocity PID slider area 
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

frame .sliderarea_vel_pid  -borderwidth $bw -relief ridge
pack .sliderarea_vel_pid  -fill x -expand true

label .l_Kp -text "Velocity PID Kp parameter          " -font $font
scale .s_Kp -from 0 -to 100 -length 500 -variable Kp \
       -tickinterval 20 -orient horizontal -troughcolor red \
       -label "                  PID Control" -font $font \
       -showvalue false -command $cmd

label .l_Ki -text "Velocity PID Ki parameter:         " -font $font
scale .s_Ki -from 0 -to 100 -length 500  -variable Ki \
       -tickinterval 20 -orient horizontal -troughcolor green \
       -font $font -showvalue false -command $cmd

label .l_Kd -text "Velocity PID Kd parameter:         " -font $font
scale .s_Kd -from 0 -to 100 -length 500 -variable Kd \
       -tickinterval 20 -orient horizontal -troughcolor blue \
       -font $font -showvalue false  -command $cmd

grid config .l_Kp -in .sliderarea_vel_pid -column 0 -row 1 -sticky "w"
grid config .s_Kp -in .sliderarea_vel_pid -column 1 -row 1 -sticky "e"

grid config .l_Ki -in .sliderarea_vel_pid -column 0 -row 2 -sticky "w"
grid config .s_Ki -in .sliderarea_vel_pid -column 1 -row 2 -sticky "e"

grid config .l_Kd -in .sliderarea_vel_pid -column 0 -row 3 -sticky "w"
grid config .s_Kd -in .sliderarea_vel_pid -column 1 -row 3 -sticky "e"

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# Describe the setpoint PID slider area 
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

frame .sliderarea_sp_pid  -borderwidth $bw -relief ridge
pack .sliderarea_sp_pid  -fill x -expand true

label .l_Kp_sp -text "Setpoint PID Kp parameter          " -font $font
scale .s_Kp_sp -from 0 -to 100 -length 500 -variable Kp_sp \
       -tickinterval 20 -orient horizontal -troughcolor red \
       -label "               Setpoint PID Control" -font $font \
       -showvalue false -command $cmd

label .l_Ki_sp -text "Setpoint PID Ki parameter:         " -font $font
scale .s_Ki_sp -from 0 -to 100 -length 500  -variable Ki_sp \
       -tickinterval 20 -orient horizontal -troughcolor green \
       -font $font -showvalue false -command $cmd

label .l_Kd_sp -text "Setpoint PID Kd parameter:         " -font $font
scale .s_Kd_sp -from 0 -to 100 -length 500 -variable Kd_sp \
       -tickinterval 20 -orient horizontal -troughcolor blue \
       -font $font -showvalue false  -command $cmd

grid config .l_Kp_sp -in .sliderarea_sp_pid -column 0 -row 1 -sticky "w"
grid config .s_Kp_sp -in .sliderarea_sp_pid -column 1 -row 1 -sticky "e"

grid config .l_Ki_sp -in .sliderarea_sp_pid -column 0 -row 2 -sticky "w"
grid config .s_Ki_sp -in .sliderarea_sp_pid -column 1 -row 2 -sticky "e"

grid config .l_Kd_sp -in .sliderarea_sp_pid -column 0 -row 3 -sticky "w"
grid config .s_Kd_sp -in .sliderarea_sp_pid -column 1 -row 3 -sticky "e"

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# Describe Text area where we can enter sine and corner freq
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

frame .textarea -borderwidth 5 -relief groove
pack .textarea -fill x -expand true

label .l_sample_period   -text  "PID sampling period (ms):  " -font $font
label .l_wheel_diam      -text  "Wheel diameter (in):  " -font $font
label .l_turn_rad        -text  "Turn radius (in):  " -font $font
label .l_tics_per_rev    -text  "Tics per revolution:  " -font $font
label .l_max_delta       -text  "Maximum delta in velocity PID loop:  " -font $font
label .l_velPIDscale     -text  "Scale factor for velocity PID parameters:  " -font $font
label .l_spPIDscale      -text  "Scale factor for setpoint PID parameters:  " -font $font

entry .e_sample_period  -textvariable   samplePeriod  -width 15 -font $font 
entry .e_wheel_diam     -textvariable   wheelDiam     -width 15 -font $font 
entry .e_turn_rad       -textvariable   turnRad       -width 15 -font $font 
entry .e_tics_per_rev   -textvariable   ticsPerRev    -width 15 -font $font 
entry .e_max_delta      -textvariable   max_delta     -width 15 -font $font 
entry .e_velPIDscale    -textvariable   velPIDscale   -width 15 -font $font 
entry .e_spPIDscale     -textvariable   spPIDscale    -width 15 -font $font 

bind .e_sample_period    <Button-1>     $cmd
bind .e_wheel_diam       <Button-1>     $cmd
bind .e_turn_rad         <Button-1>     $cmd
bind .e_tics_per_rev     <Button-1>     $cmd
bind .e_max_delta        <Button-1>     $cmd
bind .e_velPIDscale      <Button-1>     $cmd
bind .e_spPIDscale       <Button-1>     $cmd

grid config .l_sample_period  -in .textarea  -column 0 -row 0 -sticky "w"
grid config .l_wheel_diam     -in .textarea  -column 0 -row 1 -sticky "w"
grid config .l_turn_rad       -in .textarea  -column 0 -row 2 -sticky "w"
grid config .l_tics_per_rev   -in .textarea  -column 0 -row 3 -sticky "w"
grid config .l_max_delta      -in .textarea  -column 0 -row 4 -sticky "w"
grid config .l_velPIDscale    -in .textarea  -column 0 -row 5 -sticky "w"
grid config .l_spPIDscale     -in .textarea  -column 0 -row 6 -sticky "w"

grid config .e_sample_period  -in .textarea  -column 1 -row 0 -sticky "e"
grid config .e_wheel_diam     -in .textarea  -column 1 -row 1 -sticky "e"
grid config .e_turn_rad       -in .textarea  -column 1 -row 2 -sticky "e"
grid config .e_tics_per_rev   -in .textarea  -column 1 -row 3 -sticky "e"
grid config .e_max_delta      -in .textarea  -column 1 -row 4 -sticky "e"
grid config .e_velPIDscale    -in .textarea  -column 1 -row 5 -sticky "e"
grid config .e_spPIDscale     -in .textarea  -column 1 -row 6 -sticky "e"

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# Need a "Apply" and "Exit" button
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

frame .footerarea -borderwidth 5 -relief groove
pack  .footerarea 

button .b_run -text "Apply" -font $font -fg black -relief raised -height 1 \
       -command {reportState ; set applyPressed 1 ; $cmd } 

pack .b_run -in .footerarea  -side left

.b_run config -bg red

button .b_exit -text "Exit" -font $font -fg black -relief raised -height 1 \
        -command {set exitFlag 1; reportState ; exit}

pack .b_exit -in .footerarea

#
# Set up grid for resizing.
#
#grid columnconfigure . 1 -weight 1
#grid columnconfigure . 3 -weight 1

