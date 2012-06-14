<?php
echo "<p><a href='#' id='backToList'>".ALL_DEVICES."</a> &gt; <a href='#' id='backToPresetList'>".IPP_HEADER." <b>{$camera->info['device_name']}</b></a> &gt; ".IPP_EDIT." <b>{$preset->preset['name']}</b></p>";
echo "<form id='settingsForm' method='post' action='/ajax/ptzpresetedit.php?id=".((!empty($preset->preset['id'])) ? $preset->preset['id'] : 'new')."&ref={$camera->info['id']}'>";
echo "<INPUT type='hidden' id='ref' name='ref' value='{$camera->info['id']}'>";
echo "<INPUT type='hidden' id='presetId' name='presetId' value='".((!empty($preset->preset['id'])) ? $preset->preset['id'] : 'new')."'>";
echo "<div id='message' class='INFO'>".IPP_EDIT_HELP."</div>";
echo "<div id='saveButton'>".SAVE."</div>";
echo "<div><label>".IPP_PRESET_NAME."<span class='sub'>".IPP_PRESET_NAME_EX."</span></label><input type='text' name='name' value='{$preset->preset['name']}' /><br /><br /></div>";

echo "<div><label>".IPP_PRESET_RIGHT."<span class='sub'></span></label><input type='text' name='mright' value='{$preset->preset['mright']}' /></div>";
echo "<div><label>".IPP_PRESET_LEFT."<span class='sub'></span></label><input type='text' name='mleft' value='{$preset->preset['mleft']}' /></div>";
echo "<div><label>".IPP_PRESET_UP."<span class='sub'></span></label><input type='text' name='up' value='{$preset->preset['up']}' /></div>";
echo "<div><label>".IPP_PRESET_DOWN."<span class='sub'></span></label><input type='text' name='down' value='{$preset->preset['down']}' /></div>";
echo "<div><label>".IPP_PRESET_UPRIGHT."<span class='sub'></span></label><input type='text' name='up_right' value='{$preset->preset['up_right']}' /></div>";
echo "<div><label>".IPP_PRESET_UPLEFT."<span class='sub'></span></label><input type='text' name='up_left' value='{$preset->preset['up_left']}' /></div>";
echo "<div><label>".IPP_PRESET_DOWNRIGHT."<span class='sub'></span></label><input type='text' name='down_right' value='{$preset->preset['down_right']}' /></div>";
echo "<div><label>".IPP_PRESET_DOWNLEFT."<span class='sub'></span></label><input type='text' name='down_left' value='{$preset->preset['down_left']}' /></div>";
echo "<div><label>".IPP_PRESET_WIDE."<span class='sub'>".IPP_PRESET_WIDE_EX."</span></label><input type='text' name='wide' value='{$preset->preset['wide']}' /></div>";
echo "<div><label>".IPP_PRESET_TIGHT."<span class='sub'>".IPP_PRESET_TIGHT_EX."</span></label><input type='text' name='tight' value='{$preset->preset['tight']}' /></div>";
echo "<div><label>".IPP_PRESET_FOCUSIN."<span class='sub'></span></label><input type='text' name='focus_in' value='{$preset->preset['focus_in']}' /></div>";
echo "<div><label>".IPP_PRESET_FOCUSOUT."<span class='sub'></span></label><input type='text' name='focus_out' value='{$preset->preset['focus_out']}' /></div>";
echo "<div><label>".IPP_PRESET_PRESET_SAVE."<span class='sub'></span></label><input type='text' name='preset_save' value='{$preset->preset['preset_save']}' /></div>";
echo "<div><label>".IPP_PRESET_PRESET_GO."<span class='sub'></span></label><input type='text' name='preset_go' value='{$preset->preset['preset_go']}' /></div>";
echo "<div><label>".IPP_PRESET_STOP."<span class='sub'>".IPP_PRESET_STOP_EX."</span></label><input type='text' name='stop' value='{$preset->preset['stop']}' /></div>";
echo "<div><label>".IPP_PRESET_NEEDS_STOP."<span class='sub'>".IPP_PRESET_NEEDS_STOP_EX."</span></label><input type='checkbox' name='needs_stop' ".(($preset->preset['needs_stop']==1) ? 'checked' : '')."></div>";
echo "<div><label>".IPP_PRESET_HTTP_AUTH."<span class='sub'>".IPP_PRESET_HTTP_AUTH_EX."</span></label><input type='checkbox' name='http_auth' ".(($preset->preset['http_auth']==1) ? 'checked' : '')." /></div>";
echo "<div><label>".IPP_PRESET_PORT."<span class='sub'>".IPP_PRESET_PORT_EX."</span></label><input type='text' name='port' value='{$preset->preset['port']}' /></div>";
?>
</form>