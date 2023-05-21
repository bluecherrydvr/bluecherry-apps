<?php


class health extends Controller {

    private $password;
    private $username;
    private $mysqlHost;
    private $dbname;
    private $accessToken;

    public function __construct()
    {
        parent::__construct();
        // check Access Token
        $this->accessToken = $this->getAccessToken();

        // Check the access token here
        if (!$this->isAccessTokenValid()) {
            http_response_code(401);
            echo json_encode(['status' => 'Unauthorized']);
            exit();
        }

        list($dbname, $user, $password, $host) = database::read_config();
        $this->dbname = stripslashes($dbname);
        $this->username = stripslashes($user);
        $this->password = stripslashes($password);
        $this->mysqlHost = stripslashes($host);

    }

    public function getping()
    {
        http_response_code(200);
        echo json_encode(['status' => 'ok']);

//        if (!is_null($this->mysqlHost) && !is_null($this->username) && !is_null($this->password)) {
//            // Step 2: Ping the MySQL host
//            $mysqli = @new mysqli($this->mysqlHost, $this->username, $this->password);
//
//            if ($mysqli->connect_errno) {
//                // Handle connection errors
//                if ($mysqli->connect_errno == 2002) {
//                    // MySQL service unavailable
//                    http_response_code(503);
//                    echo json_encode(['status' => 'service-unavailable-error']);
//                } elseif ($mysqli->connect_errno == 2003) {
//                    // Bad gateway
//                    http_response_code(502);
//                    echo json_encode(['status' => 'bad-gateway-error']);
//                } else {
//                    // Gateway timeout or other errors
//                    http_response_code(504);
//                    echo json_encode(['status' => 'gateway-timeout-error']);
//                }
//            } else {
//                // Connection successful
//                http_response_code(200);
//                // Close the database connecion
//                $mysqli->close();
//                echo json_encode(['status' => 'ok']);
//            }
//        } else {
//            // Handle the case when MySQL host information is not found in the configuration file
//            http_response_code(503);
//            echo json_encode(['status' => 'service-unavailable-error']);
//        }

    }

    public function getlive() {
        $excessiveCpuUsage = $this->checkResourceConsumption();
        $longRunningQueries = $this->checkLongRunningQueries();
        if($excessiveCpuUsage) {
            http_response_code(503);
            echo json_encode(['status' => 'Excessive resource consumption detected']);
        }
        if($longRunningQueries) {
            http_response_code(503);
            echo json_encode(['status' => 'Long-running queries detected in the DB component.']);
        }

        http_response_code(200);
        echo json_encode(['status' => 'ok']);
    }

    public function getdb()
    {
        // Ensure query does not contain any threats or potentially harmful content
        $query = filter_input(INPUT_GET, 'query', FILTER_SANITIZE_STRING);
        if (!is_null($this->mysqlHost) && !is_null($this->username) && !is_null($this->password)) {
            $mysqli = new mysqli($this->mysqlHost, $this->username, $this->password);

            // Handle connection errors
            if ($mysqli->connect_errno) {
                if ($mysqli->connect_errno == 2002) {
                    // MySQL service unavailable
                    http_response_code(503);
                    echo json_encode(['status' => 'service-unavailable-error']);
                } elseif ($mysqli->connect_errno == 2003) {
                    // Bad gateway
                    http_response_code(502);
                    echo json_encode(['status' => 'bad-gateway-error']);
                } else {
                    // Gateway timeout or other errors
                    http_response_code(504);
                    echo json_encode(['status' => 'gateway-timeout-error']);
                }
            }

            $mysqli->autocommit(false); // Start a transaction

            // Execute your MySQL query within the transaction
            $result = $mysqli->query($query);

            // Check if the query execution was successful
            if ($result === false) {
                http_response_code(503);
                echo json_encode(['status' => 'Query execution failed!']);
            } else {
                // Retrieve profiling information
                $profileResult = $mysqli->query('SHOW PROFILES');
                $isResourceIntensive = false;

                while ($row = $profileResult->fetch_assoc()) {
                    // Get the duration of the query execution
                    $duration = $row['Duration'];
                    // Check if duration exceeds a threshold (e.g., 2 seconds)
                    if ($duration > 2) {
                        // Query is resource-intensive
                        $isResourceIntensive = true;
                        break;
                    }
                }

                // Check if the query is resource-intensive
                if ($isResourceIntensive) {
                    http_response_code(503);
                    echo json_encode(['status' => 'Query is resource-intensive!']);
                } else {
                    echo "Query is NOT resource-intensive!";
                }
            }

            // Roll back the changes made during the transaction
            $mysqli->rollback();

            // Close the database connection
            $mysqli->close();
            http_response_code(200);
            echo json_encode(['status' => 'ok']);
        }
    }


    public function getready()
    {
        // Perform sanity checks
        $initialDbSeedDone = $this->checkInitialDbSeed(); // Perform check for initial DB seed completion
        $dbConnectionWorking = $this->checkDbConnection();

        if ($initialDbSeedDone && $dbConnectionWorking) {
            // All sanity checks passed, component is ready
            http_response_code(200);
            echo json_encode(["status" => "ready"]);
        } elseif (!$initialDbSeedDone) {
            // Initial DB seed error
            http_response_code(503);
            echo json_encode(["status" => "initial-db-seed-error"]);
        } elseif (!$dbConnectionWorking) {
            // DB connection down error
            http_response_code(503);
            echo json_encode(["status" => "db-connection-error"]);
        } else {
            // Some other factor error
            http_response_code(503);
            echo json_encode(["status" => "unidentified-factor-error"]);
        }
    }

    function checkInitialDbSeed()
    {
        $initialDbSeedDone = false;
        $seededConn = @new mysqli($this->mysqlHost, $this->username, $this->password);
        mysqli_select_db($seededConn, $this->dbname);

        $seededTablesQuery = "SHOW TABLES";
        $seededTablesResult = mysqli_query($seededConn, $seededTablesQuery);

        $seededTableNames = [];
        while ($row = mysqli_fetch_row($seededTablesResult)) {
            $seededTableNames[] = $row[0];
        }
        if(count($seededTableNames > 2)) {
            $initialDbSeedDone = true;
        }

        mysqli_close($seededConn);
        return $initialDbSeedDone;
    }

    function checkDbConnection()
    {
        $initialDbSeedDone = false;
        $mysqli = @new mysqli($this->mysqlHost, $this->username, $this->password, $this->dbname);
        if (!$mysqli->connect_errno) {
            $initialDbSeedDone = true;
        }
        mysqli_close($mysqli);
        return $initialDbSeedDone;
    }

    function checkResourceConsumption()
    {
        // Get CPU and memory usage for the DB component
        $resourceConsumption = false;
        $cpuUsage = $this->getCpuUsage();
        $memoryUsage = $this->getMemoryUsage();
        $mysqlCpuUsage = $this->getMySqlCpuUsage();

        $cpuThreshold = 80; // 80% CPU usage threshold
        $memoryThreshold = 512; // 512 MB memory usage threshold

        // Check if CPU or memory usage exceeds the thresholds

        if ($cpuUsage > $cpuThreshold || $memoryUsage > $memoryThreshold || $mysqlCpuUsage > 10) {
            // Excessive resource consumption detected
            $resourceConsumption = true;
        }

        return $resourceConsumption;
    }

    function checkLongRunningQueries() {
        $mysqli = @new mysqli($this->mysqlHost, $this->username, $this->password);
        $longRunningQueries = false;
        // Check if the connection was successful
        if ($mysqli->connect_errno) {
            die('Failed to connect to MySQL: ' . $mysqli->connect_error);
        }

        // Execute the query to retrieve information about long-running queries
        $query = "SELECT * FROM INFORMATION_SCHEMA.PROCESSLIST WHERE COMMAND != 'Sleep' AND TIME > 10";
        $result = $mysqli->query($query);

        // Check if there are any long-running queries
        if ($result->num_rows > 0) {
            $longRunningQueries = true;
        }

        $mysqli->close();
        return $longRunningQueries;
    }

    function getMySqlCpuUsage() {
        $command = 'ps aux | grep mysqld | grep -v grep | awk \'{print $3}\'';
        $cpuUsage = shell_exec($command);

        // Check if the command execution was successful
        if ($cpuUsage === null) {
            die('Error executing command: ' . $command);
        }

        // Trim any leading/trailing whitespaces and return the CPU usage
        return trim($cpuUsage);
    }

    function getCpuUsage()
    {
        $cpuUsage = 0;
        $load = sys_getloadavg();

        if (is_array($load) && count($load) >= 3) {
            $cores = intval(trim(exec('nproc')));

            if ($cores > 0) {
                $cpuUsage = ($load[0] / $cores) * 100;
            }
        }

        return $cpuUsage;
    }

    function getMemoryUsage()
    {
        return memory_get_usage(true) / 1024 / 1024; // Convert bytes to MB
    }

    public function postData()
    {
        //
    }

    public function getData()
    {
        //
    }

    private function getAccessToken() {
        if (isset($_SERVER['HTTP_AUTHORIZATION'])) {
            $authorizationHeader = $_SERVER['HTTP_AUTHORIZATION'];
            return  str_replace('Bearer ', '', $authorizationHeader);
        }
        return null;
    }

    private function isAccessTokenValid(): bool
    {
        $configFile = '/etc/bluecherry.conf';
        $fileContents = file_get_contents($configFile);
        $accessTokenPattern = '/access_token\s*=\s*"([^"]+)"/';
        preg_match($accessTokenPattern, $fileContents, $matches);
        $validAccessTokens = isset($matches[1]) ? [$matches[1]] : [];
        return in_array($this->accessToken, $validAccessTokens);
    }
}

