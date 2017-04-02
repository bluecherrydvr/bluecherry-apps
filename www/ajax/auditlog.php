<?php 

class auditlog extends Controller {
	public $auditlog;
	
    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.auditlog');

        $this->view->auditlog = data::query("SELECT AuditLogs.id as id, dt, Users.username as user,
						Devices.device_name as device, AuditEventTypes.description as event_type, AuditLogs.ip as ip
						FROM AuditLogs
						LEFT JOIN AuditEventTypes ON (AuditLogs.event_type_id = AuditEventTypes.id)
						LEFT JOIN Users ON (AuditLogs.user_id = Users.id)
						LEFT JOIN Devices ON (AuditLogs.device_id = Devices.id)
						ORDER BY dt DESC LIMIT 10");
    }

    public function postData()
    {
    }
}

