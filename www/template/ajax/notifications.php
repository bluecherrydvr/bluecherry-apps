<span id="notif-status-enabled" style="display: none;"><i class="fa fa-warning fa-fw"></i> <?php echo L_ENABLE; ?></span>
<span id="notif-status-disabled" style="display: none;"><i class="fa fa-warning fa-fw"></i> <?php echo L_DISABLE; ?></span>

<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo NTF_HEADER; ?>
    
        <ol class="breadcrumb">
            <li class="active"><?php echo NTF_SUBHEADER; ?></li>
        </ol>
        </h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div class="panel panel-default">
            <div class="panel-heading"><?php echo NTF_ADD_RULE_TITLE; ?></div>
            <div class="panel-body">
        <?php
            echo $form_rule->render();
        ?>
            </div>
        </div>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div class="panel panel-default">
            <div class="panel-heading"><?php echo NTF_EXISTING_RULES; ?></div>
            <div class="panel-body">


            <?php 
        	if (empty($notifications)){
        		echo NTF_NO_RULES;
        	} else {
        		foreach($notifications as $id => $notification){
        			#prepare hours
        			$notification['start'] = str_pad($notification['s_hr'], 2, '0', STR_PAD_LEFT).':'.str_pad($notification['s_min'], 2, '0');
        			$notification['end'] = str_pad($notification['e_hr'], 2, '0', STR_PAD_LEFT).':'.str_pad($notification['e_min'], 2, '0');
        			#prepare days
        			$notification['day'] = str_split($notification['day']);
        			$tmp = '';
        			foreach($notification['day'] as $day){
        				$tmp .= constant('DW_'.$day).', ';
        			}
        			$notification['day'] = trim($tmp, ', ');
        			#prepare users
        			$notification['users'] = trim($notification['users'], '|');
        			$notification['users'] = explode('|', $notification['users']);
        			$tmp = '';
        			foreach($notification['users'] as $id){
        				$tmp .= $users[$id]['name'].'<br />';
        			}
        			$notification['users'] = $tmp;
        			#prepare cameras
        			$notification['cameras'] = trim($notification['cameras'], '|');
        			$notification['cameras'] = explode('|', $notification['cameras']);
        			$tmp = '';
        			foreach($notification['cameras'] as $id){
        				$tmp .= (isset($cameras[$id]['device_name']) ? $cameras[$id]['device_name'] : 'deleted').'<br />';
        			}
        			$notification['cameras'] = $tmp;
			
        			#output the rule
        	?>		

                        <div class="row">
                            <div class="col-lg-12 notif-rule-item" style="padding-bottom: 30px;" data-id="<?php echo $notification['id']; ?>">
                            <?php 
                                $info_rule = new View('notifications.info_rule');
                                $info_rule->notification = $notification;
                                echo $info_rule->render();
                            ?>
                            </div>
                        </div>
            <?php
            	}
            }
            ?>

            </div>
        </div>
    </div>
</div>

