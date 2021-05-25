
<div class="row" style="padding: 20px 0;">
    <div class="col-md-offset-4 col-md-4">

        <?php if (isset($_GET['status'])) {

            if (!empty($_GET['status'])) {
                echo '<div class="alert alert-success" role="alert">Configuration successful</div>';
            } else {
                echo '<div class="alert alert-success" role="alert">An error occured during configuration save</div>';
            }

        } ?>

        <div class="panel panel-default" style="margin-top:20px;">
            <div class="panel-heading"><?php echo G_SUBDOMAIN_TITLE; ?></div>
            <div class="panel-body">
                <form class="form-horizontal" method="post" id="subdomain-provider-register" action="/subdomainprovider">
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
                    <?php if(!empty($licenseIdExists)) { ?>
                    <div class="form-group">
                        <div class="col-sm-4 text-right">
                            <label for="server_ip_address_4" class="control-label">Server IP Address (IPv4):</label>
                            <br /><em>Mandatory</em>
                        </div>
                        <div class="col-sm-8 validation-field">
                            <input type="text" class="form-control" name="server_ip_address_4" id="server_ip_address_4"
                                   value="<?php echo $actualIpv4Value; ?>" required />
                        </div>

                    </div>
                    <div class="form-group">
                        <div class="col-sm-4 text-right">
                            <label for="server_ip_address_6" class="control-label">Server IP Address (IPv6):</label>
                            <br /><em>Optional</em>
                        </div>
                        <div class="col-sm-8 validation-field">
                            <input type="text" class="form-control" name="server_ip_address_6" id="server_ip_address_6"
                                   value="<?php echo $actualIpv6Value; ?>" />
                        </div>

                    </div>
                    <div class="form-group">
                        <div class="col-sm-offset-4 col-sm-8">
                            <button type="submit" class="btn btn-primary">Book</button>
                        </div>
                    </div>
                    <?php } else { ?>
                        <div class="form-group">
                            <div class="col-sm-offset-4 col-sm-8">
                                <button type="submit" class="btn btn-primary">Query</button>
                            </div>
                        </div>
                    <?php } ?>
                </form>
            </div>
        </div>
    </div>
</div>

<script type="text/javascript">
    var licenseIdExists = <?php echo !empty($licenseIdExists) ? 'true' : 'false'; ?>
</script>