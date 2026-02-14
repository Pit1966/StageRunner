## Tips and tricks
---------------------------------------------------------------------------------------------------
### Meaning of FX attributes in SEQUENCE MODE.

- Automatic next step execution on "Start next FX" : 
> Set preDelay to -1 and postDelay to desired pause time in ms. This will execute next FX in sequence automatically when started by sequence execute button.

- Prevent current FX of being stopped when "Start next FX" was clicked or executed:
> Set Hold time of FX that must not be stopped to -1.


### FxScript commands:
#### START
> Start FX item from global FX list:
>> **START { id *FX_ID* | name *FX_NAME* | *FX_NAME_OR_ID* }**

> Start FX by name:
>> **START name *FX_NAME***

> Start FX by ID (given in list ID column)
>> **START id *ID***

- If neither "name" nor "id" is given, the parameter is handled as ID if it is a number, otherwise FX name will be assumed
- You may use wildcard char '\*' in order to start multiple FX items with matching names. But only in this fashion:
>  *START \*NAME_END*  or *START NAME_START\** or *START \*NAME_PART\**

#### STOP
> Stop running FX item from global FX list
>> *STOP { id FX_ID | name FX_NAME | FX_NAME_OR_ID }*

#### FADEIN
> Fade in FxScene (DMX light scene) from global FX list
>> *FADEIN { id FX_SCENE_ID | name FX_SCENE_NAME | FX_SCENE_NAME_OR_ID } [FADE_IN_TIME]*

- If FADE_IN_TIME is not given, the command will use the default fadein time from FxScene item.

#### FADEOUT
> Fade out FxScene (DMX light scene) from global FX list, if it is on stage
>> *FADEOUT { id FX_SCENE_ID | name FX_SCENE_NAME | FX_SCENE_NAME_OR_ID | all } [FADE_OUT_TIME]*

- If FADE_OUT_TIME is not given, the command will use the default fadein time from FxScene item.

#### DMXMERGE
> 	Set DMX merge mode of a connected YADI DMX interface to the given merge mode.
The merge mode controls how the different DMX inputs are merged and routed to the DMX output of the interface.
>> *DMXMERGE INPUT_NUM MERGE_MODE*

>> | MERGE_MODE | What it does |
>> | :------: | --- |
>> |htp| highest takes precedence. For each DMX channel the highest value from all inputs are set in the output|
>> |dmxin| the DMX input will be routed to the DMX output. Computer (USB) output is switched off|
>> |usb| the computer (USB) output is routed to the DMX output of the interface. DMX input is off|

#### GRAPSCENE
> Grab current DMX input or output and copy the universe data to specified FX scene
>> **GRAPSCENE { in | out } *FX_SCENE_NAME_OR_ID***

> Example:
>> This example script shows how the StageRunner application takes over the control of a lightdesk, fades out the current lightdesk output, activate a FX scene and than gives control back to lightdesk

<pre>
    grapscene in grap_scene	# copy current DMX input to FX scene with name grap_scene (must exist)
    fadein grap_scene		# fade in the grap_scene
    wait 10ms
    dmxmerge 1 usb		# switch to exclusive PC control. Deactivate lightdesk
    fadein light_vision 1000ms	# fade in some arbitrary special scene
    fadeout grap_scene 2000ms	# fade out the grabed scene. This fades out the original lightdesk DMX output
    wait 24s
    fadeout light_vision 1000ms	# fade out the special scene
    fadein grap_scene 1000ms	# fade in the grabed scene. This shows the previous output of the lightdesk again
    wait 2s
    dmxmerge 1 htp		# reactivate lightdesk
    fadeout grap_scene		# fadeout grap_scene. Control is now completly back to the lightdesk
</pre>

#### BLACK
> Black or fadeout FX scenes (DMX) or Video playback or all together.
>> **BLACK { video | scene | light | all } [*FADE_OUT_TIME*]**

> If *FADE_OUT_TIME* is omitted, a sudden black will be done

#### REMOTE
> Execute script commands on remote StageRunner instances available via network connection. The commands are send with UDP network protocol.
>> **REMOTE *REMOTE_HOST_ADDRESS*  *SCRIPT_COMMAND***

> - *REMOTE_HOST_ADDRESS* may be an IP address or a hostname. (Maybe IP is better, since it don't needs name resolution)
> - *SCRIPT_COMMAND* is any command described here

#### FADEVOL
> Fade given audio slot to arbitrary volume.
>> **FADEVOL slot *AUDIO_SLOT_NUMBER* { *TARGET_VOLUME_ABS*|*TARGET_VOLUME_PERCENT*% } [*FADE_TIME*]**

> Use absolute volume scale (0-100): e.g. **FADEVOL 1 70**  
> or use percentage volume scale: e.g. **FADEVOL 1 70%**  
> (for now absolute and percentage is the same, but absolute volume scale may change in future)  
> You may provide a *FADE_TIME* also

> Stop all running audio effects:
>> **FADEVOL stopaudio [*FADE_TIME*]**

>> If *FADE_TIME* is given, all audio effects will be faded to 0 in the given time interval.

#### MODE
> Set one or more script execution modes
>> **MODE [breakoncancel][,disablemultistart][,enablemultistart]**

>> - **breakoncancel**: Stopping a script will immediately stop all commands inside the script. Otherwise the script will be stopped after the command has finished.
>> - **enablemultistart** and **disablemultistart**: Set if multiple start of FX script is allowed or not


#### DEFAULT
> Set default options for current executed fx(scene)
>> **DEFAULT {universe|uni|staticscene|fadetime}**
>> - **universe *UNIVERSE_NUM*** : set the default used universe
>> - **staticscene *SCENE_NUM*** : set the unvisible static scene used by default. e.g. in DMX command
>> - **fadetime *FADE_TIME***	: defines default fadein/fadeout time

#### WAIT
> Pauses the script for the given time.  
> This is an essential feature in order to control the timing of the script commands.
>> **WAIT *TIME***

> Example: This starts the FX item with id 5 and 5 seconds later it starts the FX item with id 3

    START id 5
    wait 5s
    START id 3

#### LOOP
> Allows to loop a script for the given number times.
>> **LOOP [*LINE_NUMBER*] [*LOOP_COUNT*]**

> Reaching the LOOP command line, the script will jump to the given LINE_NUMBER, increment the loop counter and break the loop if LOOP_COUNT is reached


#### PAUSE
> Stop execution of script at line position the PAUSE command appears and set the FX script item into paused mode.  
> Starting the script again (e.g. by key press, double click or other command) the script will continue execution.
>> **PAUSE**

#### DMX
> Modify static invisible scene *SCENE* by manipulation arbitrary DMX channel values
>> **DMX [*UNIVERSE*::][*SCENE*:]*CHANNEL* *VALUE* [*FADE_TIME*]**
>>
>> Set DMX channel *CHANNEL* in *UNIVERSE* to value *VALUE* with optional fade time *FADE_TIME*
>>> *CHANNEL* can be 1 - 512  
>>> *VALUE* can be 0 - 255  
>>> *SCENE* 1 - 4 (We have 4 static scenes)  
>>> *UNIVERSE* 1 - 4 (We have 4 DMX universes for now)  
>> Note: The default SCENE and UNIVERSE is 1, but can be changed with script command "DEFAULT"  
> See **DEFAULT** command in order to set default *SCENE* number or *UNIVERSE*

