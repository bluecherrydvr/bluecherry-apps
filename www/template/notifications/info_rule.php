
                            <div class="col-lg-9 col-md-9 notif-rule-item-info">
                                <table class="table table-bordered <?php echo (($notification['disabled'] == true) ? 'table-disabled' : ''); ?>">
                                    <tbody>
                                        <tr>
                                            <td class="col-lg-3 col-md-3">
                                                <?php echo NTF_T_TO; ?>
                                            </td>
                                            <td class="col-lg-9 col-md-9">
                                                <?php echo $notification['users']; ?>
                                            </td>
                                        </tr>
                                        <tr>
                                            <td>
                                                <?php echo NTF_T_DOW; ?>
                                            </td>
                                            <td>
                                                <?php echo $notification['day']; ?>
                                            </td>
                                        </tr>
                                        <tr>
                                            <td>
                                                <?php echo NTF_T_TIME; ?>
                                            </td>
                                            <td>
                                                <?php echo $notification['start']." - ".$notification['end']; ?>
                                            </td>
                                        </tr>
                                        <tr>
                                            <td>
                                                <?php echo NTD_T_CAMS; ?>
                                            </td>
                                            <td>
                                                <?php echo $notification['cameras']; ?>
                                            </td>
                                        </tr>
                                        <tr>
                                            <td>
                                                <?php echo NTD_T_LIM; ?>
                                            </td>
                                            <td>
                                                <?php echo $notification['nlimit']; ?>
                                            </td>
                                        </tr>
                                    </tbody>
                                </table>
                            </div>
                            <div class="col-lg-3 col-md-3">
                                <button type="button" class="btn btn-danger btn-block click-event" data-function="notifDelRule"><i class="fa fa-times fa-fw"></i> <?php echo L_DELETE; ?></button>
                                <button type="button" class="btn btn-primary btn-block click-event" data-function="notifEditRule"><i class="fa fa-pencil fa-fw"></i> <?php echo L_EDIT; ?></button>
                                <button type="button" class="btn btn-success btn-block click-event" data-function="notifChangeStatus"><i class="fa fa-warning fa-fw"></i> <?php echo (($notification['disabled'] == true) ? L_ENABLE : L_DISABLE); ?></button>
                            </div>
