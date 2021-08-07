
<div class="row" style="padding: 20px 0;">
    <div class="col-lg-offset-2 col-lg-8 col-md-offset-0 col-md-12">
        <div class="alert alert-info" role="alert">By default, Bluecherry uses the standard 'snakeoil' automated SSL 
        certificates installed by your operating system. These keys are inherently not secure as there is no third party 
        authentication to validate the keys.
        <br>
        Starting in 3.1.0 Bluecherry supports automatic creation of secure (TLS) certificates via Lets Encrypt (letsencrypt.
        org). Additionally, we provide you a cname DNS entry (i.e - smithcams.bluecherry.app) that is associated with your 
        purchased license key.
        <br>
        To enable this, you need the following:
            <ul>
                <li>At least port 7001 opened to the outside world on your firewall.</li>
                <li>An email address for LetsEncrypt to contact you if there is some issue (we do not collect this email address).</li>
                <li>A paid Bluecherry license, the community edition is not supported in this automated setup.</li>
            </ul>
        </div>

        <?php if (isset($_GET['status'])) {

            if (!empty($_GET['status'])) {

                switch (intval($_GET['status'])) {
                    case 1:
                        echo '<div class="alert alert-success" role="alert">Configuration successful.</div>';
                        echo '<div class="alert alert-success" role="alert">Updating subdomain certificate successful!</div>';
                        break;
                    case 2:
                        echo '<div class="alert alert-info" role="alert">Destroy config successful.</div>';
                        break;
                    case 3:
                        echo '<div class="alert alert-success" role="alert">Configuration successful.</div>';
                        echo '<div class="alert alert-danger" role="alert">An error occured during updating subdomain certificate!</div>';
                        break;
                }

            } else {
                echo '<div class="alert alert-danger" role="alert">An error occured during operaion!
                        <br>Please check that the relevant API is available or network connection.
                      </div>';
            }

        } ?>

        <form class="form-horizontal" method="post" id="subdomain-provider-register" action="/subdomainprovider">

        <div class="panel panel-default" style="margin-top:20px;">
            <div class="panel-heading"><?php echo G_SUBDOMAIN_TITLE; ?></div>
            <div class="panel-body">
                <div class="row">
                    <div class="col-md-12 col-lg-6">
                    <div class="form-group">
                        <div class="col-sm-4 text-right">
                            <label for="subdomain_name" class="control-label">Subdomain Name:</label>
                            <br /><em>Mandatory</em>
                        </div>
                        <div class="col-sm-8 validation-field">
                            <div class="input-group">
                                <input type="text" class="form-control" name="subdomain_name" id="subdomain_name"
                                       value="<?php echo $actualSubdomain; ?>" required minlength="3" maxlength="50" />
                                <span class="input-group-addon">.bluecherry.app</span>
                            </div>
                        </div>
                    </div>
                    </div>
                    <?php if(!empty($licenseIdExists)) { ?>
                    <div class="col-md-12 col-lg-6">
                    <div class="form-group">
                        <div class="col-sm-4 text-right">
                            <label for="subdomain_email" class="control-label">Email:</label>
                            <br /><em>Mandatory</em>
                        </div>
                        <div class="col-sm-8 validation-field">
                            <input type="text" class="form-control" name="subdomain_email" id="subdomain_email"
                                   value="<?php echo $actualEmail; ?>" required />
                        </div>
                    </div>
                    </div>
                </div><!-- end of row div -->
                    <div class="form-group">
                        <div class="col-sm-4 col-md-2 text-right">
                            <label for="server_ip_address_4" class="control-label">Server (IPv4) Address:</label>
                            <br /><em>Mandatory</em>
                        </div>
                        <div class="col-sm-4 validation-field">
                            <input type="text" class="form-control ip-address-field" name="server_ip_address_4" id="server_ip_address_4"
                                   value="<?php echo $actualIpv4Value; ?>" required />
                        </div>
                        <div class="col-sm-6 clearfix">
                            <button class="btn btn-info btn-get-server-ip-address pull-left" type="button">Get Server Public IPv4</button>
                            <div class="checkbox-inline pull-right">
                                <label>
                                    <input type="checkbox" name="update_ip_address_4_auto" value="1" <?php echo (!empty($autoUpdateIpv4) ? 'checked="checked"' : ''); ?>>Update IPv4 Automatically
                                </label>
                            </div>
                        </div>
                    </div>
                    <div class="form-group">
                        <div class="col-sm-4 col-md-2 text-right">
                            <label for="server_ip_address_6" class="control-label">Server (IPv6) Address:</label>
                            <br /><em>Optional</em>
                        </div>
                        <div class="col-sm-4 validation-field">
                            <input type="text" class="form-control ip-address-field" name="server_ip_address_6" id="server_ip_address_6"
                                   value="<?php echo $actualIpv6Value; ?>" />
                        </div>
                        <div class="col-sm-6 clearfix">
                            <button class="btn btn-info btn-get-server-ip-address pull-left" type="button" data-type-ipv6="true">Get Server Public IPv6</button>
                            <div class="checkbox-inline pull-right">
                                <label>
                                    <input type="checkbox" name="update_ip_address_6_auto" value="1" <?php echo (!empty($autoUpdateIpv6) ? 'checked="checked"' : ''); ?>>Update IPv6 Automatically
                                </label>
                            </div>
                        </div>
                    </div>
                    <?php } else { ?>
                    </div><!-- end of row div -->
                    <?php } ?>
            </div>
            <div class="panel-footer clearfix">
                <?php if(!empty($licenseIdExists) && !empty($actualSubdomain)) { ?>
                    <div class="pull-left">
                        <button type="button" id="btn-destroy-actual-cname-config" class="btn btn-form-action btn-danger">Destroy</button>
                    </div>
                <?php }  ?>

                <div class="pull-right">
                    <button type="submit" class="btn btn-form-action btn-primary"><?php echo !empty($licenseIdExists) ? 'Book' : 'Query'; ?></button>
                </div>
            </div>
        </div>

        </form>
    </div>
</div>

<script type="text/javascript">
    var licenseIdExists = <?php echo !empty($licenseIdExists) ? 'true' : 'false'; ?>
</script>