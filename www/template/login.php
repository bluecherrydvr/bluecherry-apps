<?php defined('INDVR') or exit();  ?>

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

    <!-- MetisMenu CSS -->
    <link href="/template/bower_components/metisMenu/dist/metisMenu.min.css" rel="stylesheet">

    <!-- Custom CSS -->
    <link href="/template/dist/css/sb-admin-2.css" rel="stylesheet">

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

    <div class="container">
        <div class="row">
            <div class="col-md-4 col-md-offset-4">
                <div class="login-panel text-center">
                    <img id="login-logo" src="/img/logo.png">
                </div>
            </div>
        </div><br>
        <div class="row">
            <div class="col-md-4 col-md-offset-4">
                <div class="panel panel-default">
                    <div class="panel-heading"><?php echo LOGIN_PLEASE; ?>
                    </div>
                    <div class="panel-body">
                        <form role="form" method="post" action="/login">
                            <fieldset>
                                <div class="form-group">
                                    <input class="form-control" placeholder="<?php echo LOGIN_LOGIN; ?>" name="login" autofocus>
                                </div>
                                <div class="form-group">
                                    <input class="form-control" placeholder="<?php echo LOGIN_PASSWORD; ?>" name="password" type="password" value="">
                                </div>
                                <!-- Change this to a button or input when using this as a form -->
                                <div class="form-group">
                                    <button type="submit" class="btn btn-lg btn-success btn-block send-req-form">Login</button>
                                </div>

                                <div class="form-group">
                                    <div class="alert alert-ajax alert-danger"></div>
                                </div>
                            </fieldset>
                        </form>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <!-- jQuery -->
    <script src="/template/bower_components/jquery/dist/jquery.min.js"></script>

    <!-- Bootstrap Core JavaScript -->
    <script src="/template/bower_components/bootstrap/dist/js/bootstrap.min.js"></script>

    <!-- Metis Menu Plugin JavaScript -->
    <script src="/template/bower_components/metisMenu/dist/metisMenu.min.js"></script>

    <!-- Custom Theme JavaScript -->
    <script src="/template/dist/js/sb-admin-2.js"></script>

    <script src="/template/dist/js/main.js?v=<?php echo time(); ?>"></script>

</body>

</html>
