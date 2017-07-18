<?php 

class downloadauditlogs extends Controller {
	
    public function __construct()
    {
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {

        header('Content-Description: File Transfer');
        header('Content-Type: text/csv; charset=utf-8');
        #header('Content-Length: ' . filesize(VAR_TARRED_LOGS_TMP_LOCATION));
        header('Content-Disposition: attachment; filename=auditlog.csv');
        #readfile(VAR_TARRED_LOGS_TMP_LOCATION);

	$csv = data::query("SELECT AuditLogs.id as id, dt, Users.username as user,
                                                Devices.device_name as device, AuditEventTypes.description as event_type, AuditLogs.ip as ip
                                                FROM AuditLogs
                                                LEFT JOIN AuditEventTypes ON (AuditLogs.event_type_id = AuditEventTypes.id)
                                                LEFT JOIN Users ON (AuditLogs.user_id = Users.id)
                                                LEFT JOIN Devices ON (AuditLogs.device_id = Devices.id)
                                                ORDER BY dt DESC");


	$output = fopen('php://output', 'w');

	foreach($csv as $row) {
		fputcsv($output, array_values($row));
	}

        exit();
    }

    public function postData()
    {
    }
}


