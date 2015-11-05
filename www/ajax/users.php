<?php 

class users  extends Controller {
	public $data;
    public $all;

    public function __construct(){
        parent::__construct();
		$this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.users');

        $id = Inp::get('id');

        if (!$id) { 
            $this->data = $this->GetAllUsersInfo(); $this->all = true; 
        } else if  ($id=='new') { 
            $this->data['name'] = USERS_NEW; 
            $this->all = false; $this->data['new'] = true; $this->data[0]['emails'] = array(' : '); 
        } //enter only name
        else { 
            $this->data = $this->GetUserInfo($id); 
            $this->all = false; 
        }

        $dvr_users = new StdClass();
        $dvr_users->data = $this->data;
        $dvr_users->all = $this->all;


        $this->view->dvr_users = $dvr_users;
    }

    private function GetAllUsersInfo()
    {
		return data::getObject('Users');	
	}
	
    private function GetUserInfo($id)
    {
		$user_data = data::getObject('Users', 'id', $id);
		$tmp = explode('|', $user_data[0]['email']);
		
		foreach($tmp as $key => $value){
			$value = explode(':', $value);
			$user_data[0]['emails'][$key]['addrs'] = $value[0];
		}
		return $user_data;
	}
}

