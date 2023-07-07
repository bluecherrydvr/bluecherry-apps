<?php

class layouts extends Controller {

    public function __construct(){
        parent::__construct();
        $this->chAccess('viewer');
    }

    public function postData()
    {
        database::escapeString($_POST['layout']);
        switch (Inp::post('mode')) {
            case 'edit': $this->editLayout(); break;
            case 'new' : $this->newLayout(); break;
            case 'load': $this->loadLayout(); break;
            case 'delete': $this->deleteLayout(); break;
            case 'init': $this->initLayout(); break;
            case 'clear': $_SESSION['load_layout']='null:null;'; break;
        }
    }
    private function initLayout() {
        $layouts = data::query("SELECT layout_name, layout FROM userLayouts WHERE user_id='{$_SESSION['id']}'");

        // Initialize the layouts array in the session if it doesn't exist
        if (!isset($_SESSION['layouts'])) {
            $_SESSION['layouts'] = array();
        }

        // Iterate over the layouts and add them to the session
        foreach ($layouts as $layout) {
            $layout_name = $layout['layout_name'];
            $layout_data = $layout['layout'];

            $_SESSION['layouts'][$layout_name] = $layout_data;
        }
    }

    private function deleteLayout(){
        data::query("DELETE FROM userLayouts WHERE user_id={$_SESSION['id']} AND layout_name='{$_POST['layout']}'");
    }
    private function editLayout(){
        $layoutJson = $_POST['layout'];
        data::query("UPDATE userLayouts SET layout='{$layoutJson}' WHERE user_id={$_SESSION['id']} AND layout_name='{$_POST['layout_name']}'");

        // Update the layout in the session as well
        $_SESSION['layouts'][$_POST['layout_name']] = $layoutJson;
    }

    private function newLayout(){
        $layoutJson = $_POST['layout'];
        data::query("INSERT INTO userLayouts VALUES (NULL, {$_SESSION['id']}, '{$_POST['layout_name']}', '{$layoutJson}')");

        // Store the new layout in the session as well
        $_SESSION['layouts'][$_POST['layout_name']] = $layoutJson;
    }

    private function loadLayout(){
        // Check if the layout exists in the session
        if (!isset($_SESSION['layouts'][$_POST['layout_name']])) {
            // If not, load it from the database
            $layout = data::query("SELECT layout_name, layout FROM userLayouts WHERE layout_name='{$_POST['layout_name']}' AND user_id='{$_SESSION['id']}'");

            if($layout){
                $_SESSION['layouts'][$layout[0]['layout_name']] = $layout[0]['layout'];
            } else {
                // The requested layout was not found
                // Handle this error as needed
            }
        }

        $_SESSION['load_layout'] = $_SESSION['layouts'][$_POST['layout_name']];
        $_SESSION['load_layout_name'] = $_POST['layout_name'];
    }


    private function cookieToString($unset = false){
        $r = '';
        foreach($_COOKIE as $key => $value){
            if (preg_match("/imgSrcy[0-9]*x[0-9]*|lvRows|lvCols/", $key)) {
                $r .= "{$key}:{$value};";
                if ($unset) setcookie($key, '', 5);

            };
        }
        return $r;
    }
}

