<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header">
    
        <ol class="breadcrumb">
            <li><a href="/devices" class="ajax-content"><?php echo ALL_DEVICES; ?></a></li>
            <li class="active"> <?php echo IPP_HEADER . $camera->info['device_name']; ?></li>
        </ol>
        </h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <a href="/ptzpresetedit?id=new&ref=<?php echo $camera->info['id'];?>" class="btn btn-success pull-right ajax-content" type="submit"><i class="fa fa-plus fa-fw"></i> <?php echo IPP_NEW; ?></a>
        <div class="clearfix"></div>
    </div>
</div>
<br>

<div class="row">
    <div class="col-lg-12">

        <table class="table table-striped table-hover table-vert-align" id="ptzprelist">
            <tbody>
                <tr class="<?php echo ((empty($camera->info['ptz_control_path'])) ? 'active' : ''); ?> click-event" data-id="<?php echo $camera->info['id']; ?> " data-path="" data-function="ptzPreSetListSelect">
                    <td colspan="2">
                        <?php echo IPP_DISABLE_PTZ; ?>                 
                    </td>
                </tr>

                <?php foreach($presets as $n => $preset){ ?>
                <tr class="<?php echo (($preset['id']==$camera->info['ptz_control_path']) ? 'active' : ''); ?> click-event" data-id="<?php echo $camera->info['id']; ?> " data-path="<?php echo $preset['id'];?>" data-function="ptzPreSetListSelect">
                    <td class="col-lg-10 col-md-10">
                        <?php echo $preset['name']; ?>
                    </td>
                    <td class="col-lg-2 col-md-2 text-right">
                        <div <?php echo ((($preset['name'] == 'ONVIF') && ($preset['protocol'] == 'onvif')) ? 'style="display: none;"' : ''); ?> >
                        <a href="/ptzpresetedit?id=<?php echo $preset['id'];?>&ref=<?php echo $camera->info['id'];?>" type="submit" class="btn btn-success ajax-content"><i class="fa fa-edit fa-fw"></i></a>

                        <form action="/ajax/update.php" method="POST" style="display: inline-block;">
                            <input name="mode" value="deleteIpPtzPreset" type="hidden">
                            <input name="type" value="Devices" type="hidden">
                            <input name="id" value="<?php echo $preset['id'];?>" type="hidden">
                            <input name="do" value="true" type="hidden">

                            <button type="submit" class="btn btn-danger send-req-form" data-confirm="Are you sure you want to delete preset '<?php echo $preset['name'];?>'?" data-loading-text="..." data-func="delTr"><i class="fa fa-times fa-fw"></i></button>
                        </form>
                        </div>
                    </td>
                </tr>
                <?php } ?>
            </tbody>
        </table>

    </div>
</div>
