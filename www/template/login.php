<?php defined('INDVR') or exit();  ?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html>
<head>
	<title><?php  echo DVR_COMPANY_NAME.' '.DVR_DVR.PAGE_HEADER_SEPARATOR.PAGE_HEADER_LOGIN; ?></title>
	<link rel="stylesheet" href="template/styles.css">
	<script type="text/javascript" src="./lib/mootools.js"></script>
	<script type="text/javascript" src="./lib/mootools-more.js"></script>
	<script type="text/javascript" src="./lib/js/login.js"></script>
</head>
<body>
<div id="login-window-container">
	<div><img id="login-logo" src="/img/logo.png" /></div>
	<div id="login-window"><br />
		<FORM id="login-form" method="post" action="/ajax/login.php">
   			<img src="/img/username_header.png" /><br /><INPUT name="login" type="Text" value=""><br />
   			<img src="/img/password_header.png" /><br /><INPUT name="password" type="Password"><br />
  			<INPUT id="login-button" type="Submit" value="Login"><br /><br />
		</FORM>
		<div id="login-message"><?php if (!empty($_SESSION['message'])) echo $_SESSION['message'];?></div>
	</div>
</div>
</body>
</html>