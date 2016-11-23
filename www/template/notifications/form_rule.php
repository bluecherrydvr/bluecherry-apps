


        <form method="post" action="/ajax/notifications.php<?php echo (($notification['id']) ? '?mode=edit&id='.$notification['id'] : ''); ?>" class="form-horizontal" id="new-rule-form" role="form">
        <div class="form-group">
            <div class="col-lg-12">
                <button class="btn btn-success pull-right send-req-form" type="submit" data-redirect-success="/ajax/notifications.php"><i class="fa fa-check fa-fw"></i> <?php echo (($notification['id']) ? NTF_EDIT_RULE : NTF_ADD_RULE); ?></button>
                <div class="clearfix"></div>
            </div>
        </div>


                <div class="panel panel-default">
                    <div class="panel-heading"><?php echo NTF_ADD_RULE_DATETIME; ?></div>
                    <div class="panel-body">
                        <div class="form-group">
                            <div class="col-lg-3 col-md-3">
                				<select class="form-control" name="daysofweek[]" multiple size="7">
                					<option value='M' <?php echo (in_array('M', $notification['day'])) ? 'selected' : ''; ?>><?php echo DW_M; ?></option>
                					<option value='T' <?php echo (in_array('T', $notification['day'])) ? 'selected' : ''; ?>><?php echo DW_T; ?></option>
                					<option value='W' <?php echo (in_array('W', $notification['day'])) ? 'selected' : ''; ?>><?php echo DW_W; ?></option>
                					<option value='R' <?php echo (in_array('R', $notification['day'])) ? 'selected' : ''; ?>><?php echo DW_R; ?></option>
                					<option value='F' <?php echo (in_array('F', $notification['day'])) ? 'selected' : ''; ?>><?php echo DW_F; ?></option>
                					<option value='S' <?php echo (in_array('S', $notification['day'])) ? 'selected' : ''; ?>><?php echo DW_S; ?></option>
                					<option value='U' <?php echo (in_array('U', $notification['day'])) ? 'selected' : ''; ?>><?php echo DW_U; ?></option>
                				</select>                        
                            </div>
                            <div class="col-lg-3 col-md-3">


                                <div class="row">
                                    <div class="panel panel-default">
                                        <div class="panel-heading"><?php echo NTF_START_TIME; ?></div>
                                        <div class="panel-body">
                                                <div class="col-lg-6 ">
                                                    <select class="form-control input-sm" name='s_hr' id='input-s_hr'><?php for($i=0; $i<=23; $i++){ echo "<option value='{$i}' ".((isset($notification['s_hr']) && $i==$notification['s_hr']) ? 'selected' : '').">$i</option>"; } ?></select>
                                                </div>
                                                <div class="col-lg-6 ">
                                                    <select class="form-control input-sm" name='s_min' id='input-s_min'><?php for($i=0; $i<=59; $i++){ echo "<option value='{$i}' ".((isset($notification['s_min']) && $i==$notification['s_min']) ? 'selected' : '').">$i</option>"; } ?></select>
                                                </div>
                                        </div>
                                    </div>
                                </div>

                                <div class="row">
                                    <div class="panel panel-default">
                                        <div class="panel-heading"><?php echo NTF_END_TIME; ?></div>
                                        <div class="panel-body">
                                                <div class="col-lg-6 ">
                                					<select class="form-control input-sm" name='e_hr'><?php for($i=0; $i<=23; $i++){ echo "<option value='{$i}' ".((isset($notification['e_hr']) && $i==$notification['e_hr']) ? 'selected' : '').">$i</option>"; } ?></select>
                                                </div>
                                                <div class="col-lg-6">
                        					        <select class="form-control input-sm" name='e_min'><?php for($i=0; $i<=59; $i++){ echo "<option value='{$i}' ".((isset($notification['e_min']) && $i==$notification['e_min']) ? 'selected' : '').">$i</option>"; } ?></select>
                                                </div>
                                        </div>
                                    </div>
                                </div>

                            </div>
                            <div class="col-lg-3 col-md-3">
                                <div class="panel panel-default">
                                    <div class="panel-heading"><?php echo NTF_CAMERAS; ?></div>
                                    <div class="panel-body">
                                		<select class="form-control" name='cameras[]' multiple='' size='7' id='input-cameras'>
                                			<?php foreach($cameras as $id => $camera){ echo "<option value='{$camera['id']}' ".((in_array($camera['id'], $notification['cameras'])) ? 'selected' : '').">{$camera['device_name']}</option>"; } ?>
                                		</select>
                                    </div>
                                </div>
                            </div>
                            <div class="col-lg-3 col-md-3">
                                <div class="panel panel-default">
                                    <div class="panel-heading"><?php echo NTF_USERS; ?></div>
                                    <div class="panel-body">
                            			<select class="form-control" name='users[]' multiple='' size='7' id='input-users'>
                            				<?php foreach($users as $id => $user){ echo "<option value='{$user['id']}' ".((in_array($user['id'], $notification['users'])) ? 'selected' : '').">{$user['name']}</option>"; }?>
                            			</select>
                                    </div>
                                </div>
                            </div>

                        </div>
                    </div>
                </div>


                <div class="panel panel-default">
                    <div class="panel-heading"><?php echo NTF_LIMIT; ?></div>
                    <div class="panel-body">
                        <div class="form-group">
                            <label class="col-lg-6 control-label"><?php echo NTF_LIMIT_LABEL; ?></label>

                            <div class="col-lg-6">
                                <input class="form-control" type="text" name="limit" value="<?php echo $notification['nlimit']; ?>"  />
                            </div>
                        </div>
                    </div>
                </div>

	</form>


