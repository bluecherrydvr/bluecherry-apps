<?php defined('INDVR') or exit(); 

?>
<!DOCTYPE html>
<html lang="en">

<head>

    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="description" content="<?php echo DVR_COMPANY_NAME; ?>">
    <meta name="author" content="<?php echo DVR_COMPANY_NAME; ?>">

    <title><?php echo DVR_COMPANY_NAME.' '.DVR_DVR.PAGE_HEADER_SEPARATOR.PAGE_HEADER_MAIN; ?></title>

    <!-- Bootstrap Core CSS -->
    <link href="/template/bower_components/bootstrap/dist/css/bootstrap.min.css" rel="stylesheet">

    <!-- datetimepicker -->
    <link href="/template/bower_components/datetimepicker/css/bootstrap-datetimepicker.min.css" rel="stylesheet">

    <link href="/template/bower_components/bootstrapformhelpers/dist/css/bootstrap-formhelpers.min.css" rel="stylesheet">

    <!-- MetisMenu CSS -->
    <link href="/template/bower_components/metisMenu/dist/metisMenu.min.css" rel="stylesheet">

    <!-- Timeline CSS -->
    <link href="/template/dist/css/timeline.css" rel="stylesheet">

    <!-- Custom CSS -->
    <link href="/template/dist/css/sb-admin-2.css" rel="stylesheet">

    <!-- Morris Charts CSS -->
    <link href="/template/bower_components/morrisjs/morris.css" rel="stylesheet">

    <!-- Custom Fonts -->
    <link href="/template/bower_components/font-awesome/css/font-awesome.min.css" rel="stylesheet" type="text/css">

    <link href="/template/bower_components/animate.css" rel="stylesheet">

    <!-- player -->
    <link href="/template/bower_components/mediaelement/mediaelementplayer.css" rel="stylesheet">

    <!-- bootstrap table -->
    <link href="/template/bower_components/bootstrap-table/bootstrap-table.min.css" rel="stylesheet">

    <!-- Custom CSS -->
    <link href="/template/dist/css/main.css?v=<?php echo time(); ?>" rel="stylesheet">
    <!-- HTML5 Shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
        <script src="https://oss.maxcdn.com/libs/html5shiv/3.7.0/html5shiv.js"></script>
        <script src="https://oss.maxcdn.com/libs/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->

</head>

<body>
    <div id="wrapper">
        <!-- Navigation -->
        <nav class="navbar navbar-default navbar-static-top" role="navigation" style="margin-bottom: 0">
            <div class="navbar-header">
                <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
                    <span class="sr-only">Toggle navigation</span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                </button>
                <a class="navbar-brand" href="/">
                    <img style="max-width: 80px; margin-top: -10px;" src="/img/logo.png">
                </a>
            </div>
            <!-- /.navbar-header -->

            <ul class="nav navbar-top-links navbar-right">
                <!-- /.dropdown -->
                <li class="dropdown">
                    <a class="dropdown-toggle" data-toggle="dropdown" href="#">
                        <i class="fa fa-user fa-fw"></i>  <i class="fa fa-caret-down"></i>
                    </a>
                    <ul class="dropdown-menu dropdown-user">
                        <li>
                            <a href="/profile" class="ajax-content"><i class="fa fa-user fa-fw"></i> Profile</a>
                        </li>
                        <li>
                            <a href="/liveview"><i class="fa fa-image fa-fw"></i> LiveView</a>
                        </li>
                        <li>
                            <a href="/playback"><i class="fa fa-play fa-fw"></i> Playback</a>
                        </li>
                        <?php if ($current_user->info['access_setup']) { ?>
                        <li>
                            <a href="/"><i class="fa fa-dashboard fa-fw"></i> Admin panel</a>
                        </li>
                        <?php } ?>
                        <li>
                            <a href="/logout"><i class="fa fa-sign-out fa-fw"></i> <?php echo LOGOUT; ?></a>
                        </li>
                    </ul>
                    <!-- /.dropdown-user -->
                </li>
                <!-- /.dropdown -->
            </ul>
            <!-- /.navbar-top-links -->

            <?php if ($left_menu) { ?>
            <div class="navbar-default sidebar" role="navigation">
                <div class="sidebar-nav navbar-collapse" id="navbar-collapse">
                    <ul class="nav" id="side-menu">


                        <li>
                            <a href="javascript:void(0);" class='liveView'><i class="fa fa-image fa-fw"></i> <?php echo MMENU_LIVEVIEW; ?></a>
                        </li>
                        <li>
                            <a href="/playback"><i class="fa fa-play fa-fw"></i> <?php echo MMENU_PLAYBACK; ?></a>
                        </li>
                        <li>
                            <a href="javascript:void(0);" class='downloadClient'><i class="fa fa-download fa-fw"></i> <?php echo MMENU_CLIENT_DOWNLOAD; ?></a>
                        </li>
                        <li>
                            <a href="http://docs.bluecherrydvr.com/"><i class="fa fa-book fa-fw"></i> <?php echo MMENU_DOCUMENTATION; ?></a>
                        </li>

                        <li class="divider"></li>
                        <li class="divider"></li>

                        <li>
                            <a href="/news" data-left-menu="true"><i class="fa fa-dashboard fa-fw"></i> <?php echo MMENU_NEWS; ?>&nbsp; <div class="visible-inline-block announcekit-widget"> </div></a>
                        </li>

                        <li class="divider"></li>
                        <li class="divider"></li>

                        <li>
                            <a href="/general" class="ajax-content" data-left-menu="true"><i class="fa fa-cog fa-fw"></i> <?php echo MMENU_GS; ?></a>
                        </li>
                        <li>
                            <a href="/storage" class="ajax-content" data-left-menu="true"><i class="fa fa-hdd-o fa-fw"></i> <?php echo MMENU_STORAGE; ?></a>
                        </li>
                        <li>
                            <a href="/users" class="ajax-content" data-left-menu="true"><i class="fa fa-users fa-fw"></i> <?php echo MMENU_USERS; ?></a>
                        </li>
                        <li>
                            <a href="/activeusers" class="ajax-content" data-left-menu="true"><i class="fa fa-user-md fa-fw"></i> <?php echo MMENU_ACTIVE_USERS; ?></a>
                        </li>

                        <li class="divider"></li>
                        <li class="divider"></li>

                        <li>
                            <a href="/devices" class="ajax-content" data-left-menu="true"><i class="fa fa-video-camera fa-fw"></i> <?php echo MMENU_DEVICES; ?></a>
                        </li>
                        <li>
                            <a href="/deviceschedule" class="ajax-content" data-left-menu="true"><i class="fa fa-th fa-fw"></i> <?php echo MMENU_SCHED; ?></a>
                        </li>
                        <li>
                            <a href="/notifications" class="ajax-content" data-left-menu="true"><i class="fa fa-envelope fa-fw"></i> <?php echo MMENU_NOTFICATIONS; ?></a>
                        </li>
                        <li>
                            <a href="/webhook"><i class="fa fa-cog fa-fw"></i> Webhooks</a>
                        </li>
                        <li>
                            <a href="/subdomainprovider"><i class="fa fa-briefcase fa-fw"></i> <?php echo G_SUBDOMAIN_TITLE; ?></a>
                        </li>
                        <li class="divider"></li>
                        <li class="divider"></li>


                        <li>
                            <a href="/statistics" class="ajax-content" data-left-menu="true"><i class="fa fa-bar-chart-o fa-fw"></i> <?php echo MMENU_STATISTICS; ?></a>
                        </li>
                        <li>
                            <a href="/monitor" class="ajax-content" data-left-menu="true"><i class="fa fa-area-chart fa-fw"></i> <?php echo MMENU_MONITORING; ?></a>
                        </li>
                        <li>
                            <a href="/backup" class="ajax-content" data-left-menu="true"><i class="fa fa-database fa-fw"></i> <?php echo MMENU_BACKUP; ?></a>
                        </li>
                        <li>
                            <a href="/log" class="ajax-content" data-left-menu="true"><i class="fa fa-file-o fa-fw"></i> <?php echo MMENU_LOG; ?></a>
                        </li>

                    </ul>

                    <div class="panel-body">
                        <div id="sr" class="alert alert-success server-stat">
                            <?php echo SERVER_RUNNING; ?>
                        </div>

                        <div id="snr" class="alert alert-danger server-stat">
                            <?php echo SERVER_NOT_RUNNING; ?>
                        </div>

                        <div id="ncn" class="alert alert-danger">
                            <?php echo NO_CONNECTION; ?>
                        </div>

                        <div id="ftw" class="alert alert-danger server-stat">
                            <?php echo WRITE_FAILED; ?>
                        </div>



                        <?php
                            if (empty($global_settings->data['G_DISABLE_VERSION_CHECK']) || $global_settings->data['G_DISABLE_VERSION_CHECK']==0){
                                $version = new softwareVersion($global_settings);

                                if (!$version->version['up_to_date']){
                                    echo '<div class="alert alert-danger">'.NOT_UP_TO_DATE.'<br /><a href="/newversion" class="ajax-content">'.WANT_TO_LEARN_MORE.'</a></div>';
                                } else {
                                    echo '<div class="alert alert-success">'.UP_TO_DATE.': '.$version->version['installed'].'</div>';
                                }
                            }
                        ?>
                    </div>


                    <div id="server-stats" class="panel panel-default server-stat">
                        <div class="panel-heading">
                            <i class="fa fa-bar-chart-o fa-fw"></i> <?php echo STATS_HEARDER; ?>
                        </div>
                        <div class="panel-body">
                            <div id="stat-cpu">
                                <p>
                                    <strong><?php echo STATS_CPU; ?></strong>
                                    <span class="pull-right text-muted">0%</span>
                                </p>
                                <div class="progress">
                                    <div class="progress-bar progress-bar-success" role="progressbar" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100" style="width: 0%">
                                        <span class="sr-only">0%</span>
                                    </div>
                                </div>
                            </div>


                            <div id="stat-mem">
                                <p>
                                    <strong><?php echo STATS_MEM; ?></strong>
                                    <span class="pull-right text-muted">0%</span>
                                </p>
                                <div class="progress">
                                    <div class="progress-bar progress-bar-danger" role="progressbar" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100" style="width: 0%">
                                        <span class="sr-only">0%</span>
                                    </div>
                                </div>
                            </div>
                            <div class="text-center"><p id="stat-meminfo">123</p></div>

                            <div>
                                <strong><?php echo STATS_UPT ?></strong>
                            </div>
                            <div id="server-uptime"></div>

                        </div>
                    </div>
                </div>
            </div>
            <?php } ?>
            <!-- /.navbar-static-side -->
        </nav>

        <div id="page-wrapper" class="<?php echo ((!$left_menu) ? 'without-left-menu' : ''); ?>">

        <?php  if (!empty($general_error)){ ?>
            <div class="row general-error">
                <div class="col-lg-12">
                    <div class="alert alert-warning <?php echo $general_error['type']; ?>">
                        <span class="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span>
                        <span id="general_error"><?php echo $general_error['text']; ?></span>
                    </div>
                </div>
            </div>
        <?php } ?>

            <div id="page-container">
                <?php echo ((isset($page_container) ? $page_container : 'empty')); ?>
            </div>

        </div>

    </div>

    <!-- jQuery -->
    <script src="/template/bower_components/jquery/dist/jquery.min.js"></script>

    <!-- Bootstrap Core JavaScript -->
    <script src="/template/bower_components/bootstrap/dist/js/bootstrap.min.js"></script>

    <!-- datetimepicker -->
    <script src="/template/bower_components/datetimepicker/js/bootstrap-datetimepicker.js"></script>

    <script src="/template/bower_components/bootstrapformhelpers/js/bootstrap-formhelpers-slider.js"></script>

    <!-- Metis Menu Plugin JavaScript -->
    <script src="/template/bower_components/metisMenu/dist/metisMenu.min.js"></script>

    <!-- Morris Charts JavaScript -->
    <script src="/template/bower_components/raphael/raphael-min.js"></script>
    <script src="/template/bower_components/morrisjs/morris.min.js"></script>

    <!-- Notify -->
    <script src="/template/bower_components/bootstrap-notify.min.js"></script>

    <!-- Custom Theme JavaScript -->
    <script src="/template/dist/js/sb-admin-2.js"></script>

    <!-- player -->
    <script src="/template/bower_components/mediaelement/mediaelement-and-player.min.js"></script>

    <!-- bootstrap table -->
    <script src="/template/bower_components/bootstrap-table/bootstrap-table.min.js"></script>

    <!-- jquery validation plugin -->
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery-validate/1.19.2/jquery.validate.min.js"></script>

    <!-- Custom Theme JavaScript -->
    <script type="text/javascript" src="/vars"></script>
    <script src="/template/dist/js/main.js?v=<?php echo time(); ?>"></script>
    <script src="/template/dist/js/main_load.js?"></script>

    <script src="/template/dist/js/storage.js"></script>
    <script src="/template/dist/js/users.js"></script>
    <script src="/template/dist/js/devices.js"></script>
    <script src="/template/dist/js/addip.js"></script>
    <script src="/template/dist/js/editip.js"></script>
    <script src="/template/dist/js/ptzpresetlist.js"></script>
    <script src="/template/dist/js/notifications.js"></script>
    <script src="/template/dist/js/statistics.js"></script>
    <script src="/template/dist/js/backup.js"></script>
    <script src="/template/dist/js/log.js"></script>
    <script src="/template/dist/js/licensing.js"></script>
    <script src="/template/dist/js/videoadj.js"></script>
    <script src="/template/dist/js/general.js"></script>
    <script src="/template/dist/js/playback.js"></script>
    <script src="/template/dist/js/discoverCameras.js"></script>
    <script src="/template/dist/js/profile.js"></script>
    <script src="/template/dist/js/ptz.js"></script>
    <script src="/template/dist/js/webhook.js"></script>
    <script src="/template/dist/js/subdomainprovider.js"></script>
    <script>
        window.announcekit = (window.announcekit || { queue: [], on: function(n, x) { 
            window.announcekit.queue.push([n, x]); }, push: function(x) { window.announcekit.queue.push(x); } 
        });
        
        window.announcekit.push({
            "widget": "https://announcekit.co/widgets/v2/SznKU",
            "selector": ".announcekit-widget"
        })

        window.announcekit.push({
            "widget": "https://changelog.bluecherrydvr.com/widgets/v2/1fhia4",
            "selector": ".announcekit-widget-embed",
            "embed": true
        })
    </script>
    <script async src="https://cdn.announcekit.app/widget-v2.js"></script>

    <?php echo getJs(); ?>
</body>
</html>
