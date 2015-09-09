<?php

class View
{
	protected $path;
	public $name;
	protected $_data = array();
	protected $_extension = 'php';

	public function __construct($name)
	{
		$this->set_template($name);
	}
	
    public function set_template($name) {
        $name = str_replace('.', '/', $name);

        $this->name = $name;

        $file = SITE_PATH.'template/'.$name.'.php';
			
        if (!file_exists($file))
            throw new Exception("View {$name} - not found.");
			
		$this->path = $file;
	}

	public function __set($key, $val)
	{
		$this->_data[$key] = $val;
	}

	public function __isset($key)
	{
		return array_key_exists($key, $this->_data);
	}

	public function __get($key)
	{
		if (isset($this->_data[$key]))
			return $this->_data[$key];
		throw new Exception("Value {$key} not set for view {$this->name}");
	}

    public function getData()
    {
        return $this->_data;
    }

    public function setData($data)
    {
        $this->_data = $data;
    }

    public function flushData()
    {
        $this->_data = Array();
    }

	public function render()
	{
		if (!file_exists($this->path))
			throw new Exception("View {$this->path} not found.");

		extract($this->_data);
		ob_start();
		include($this->path);
		
		return ob_get_clean();
	}
	
}
