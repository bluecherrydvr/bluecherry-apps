<?php 

class vars extends Controller {

    public function getData()
    {
        $locale_en = localeEn();

        $res = "
            var_layoutToLoad = '".CHOOSE_LAYOUT."';
            var_email = '".USERS_EMAIL."';
            var_email_ex = '".USERS_EMAIL_EX."';
            var_del_can_conf = '".DELTE_CAM_CONF."';
            var_del_ipp_conf = '".IPP_DELETE."'
            var_rm_client_download = '".RM_CLIENT_DOWNLOAD."'
            var_check_connectivity = '".((!globalSettings::getParameter('G_DISABLE_IP_C_CHECK')) ? '1': '0')."';
            var_loading = '".U_LOADING."';
            var_backup = '".U_BACKUP."';
            var_backup_conf = '".BACKUP_R_CONF."';
            var_backup_warn= '".BACKUP_R_WARN."';
            var_confirm_remove_license = '".L_CONFIRM_DELETE."';
            var_disable = '".L_DISABLE."';
            var_enable = '".L_ENABLE."';
            var_locale_en = parseInt(".$locale_en.");
            var_date_format = '".dateFormat(time(), $locale_en)->format_js."';
        ";

        return $res;
    }
}

