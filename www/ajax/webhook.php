<?php

class webhook extends Controller {

    protected static $fields = array('label', 'url', 'events', 'cameras', 'status');

    public function __construct(){
        parent::__construct();
        $this->chAccess('admin');
    }

    public function getData()
    {
        $this->setView('ajax.webhook');

        $this->view->cameras = data::getObject('Devices');
        $this->view->webhooks = data::getObject('webhooks');
        $this->view->events = array('motion_event', 'device_state', 'solo');

        $this->setCurrentFormData();

    }

    public function postData()
    {
        if (empty($_GET['action'])) {
            data::responseJSON(false, 'Invalid type of action');
            return;
        }

        switch ($_GET['action']) {
            case 'create': $this->createWebhook(); break;
            case 'edit': $this->editWebhook(); break;
            case 'delete': $this->deleteWebhook(); break;
        }
    }

    protected function setCurrentFormData()
    {
        if (empty($_GET['action']) || $_GET['action'] !== 'edit') {
            return;
        }

        if (empty($_GET['id'])) {
            return;
        }

        $id = intval($_GET['id']);

        if (empty($id)) {
            return;
        }

        foreach ($this->view->webhooks as $target) {
            if (intval($target['id']) === $id) {
                $target['events'] =  explode(',', $target['events']);
                $target['cameras'] = explode(',', $target['cameras']);

                $this->view->currentFormData = $target;
                break;
            }
        }
    }

    protected function createWebhook()
    {
        if (!$this->validateForm()) {
            return;
        }

        $query = 'INSERT INTO `webhooks` SET ' . $this->getValues();
        $result = data::query($query, true);
        data::responseJSON(true, 'create ok', $result);
    }

    protected function editWebhook()
    {
        $id = $this->getId();

        if (!$this->validateForm()) {
            return;
        }

        $query = 'UPDATE `webhooks` SET ' . $this->getValues() . ' WHERE `id` = ' . $id;
        $result = data::query($query, true);
        data::responseJSON(true, 'update ok', $result);
    }

    protected function deleteWebhook()
    {
        $id = $this->getId();

        $query = 'DELETE FROM `webhooks` WHERE `id` = ' . $id;
        $result = data::query($query, true);
        data::responseJSON($result);
    }

    protected function getValues()
    {
        $values = array();

        foreach (self::$fields as $field) {
            if (empty($_POST[$field])) {

                if ($field === 'status') {
                    $_POST[$field] = 0;
                } else {
                    continue;
                }
            }

            $data = is_array($_POST[$field]) ? implode(',', $_POST[$field]) : $_POST[$field];
            $values[] = '`' . $field . '`=\'' . database::escapeString($data) . '\'';
        }

        return implode(',', $values);
    }

    protected function getId()
    {
        if (empty($_GET['id'])) {
            data::responseJSON(false, 'ID value is required to edit');
        }

        $id = intval($_GET['id']);

        if (empty($id)) {
            data::responseJSON(false, 'Invalid ID value');
        }

        return $id;
    }

    protected function validateForm()
    {
        if (empty($_POST['label'])) {
            data::responseJSON(false, 'Label is not defined');
            return false;
        }

        if (empty($_POST['url'])) {
            data::responseJSON(false, 'URL is not defined');
            return false;
        }

        return true;
    }

}


