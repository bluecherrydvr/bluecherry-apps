<?php

/**

 * @author Lorenzo Toscano <lorenzo.toscano@gmail.com>

 * @version 1.0

 * @desc A simple class to help you in developing an ONVIF compliant client

**/

class Ponvif {

    /*
        $ipaddress  ip address of the NVT device
        $username   NVT authentication username
        $password   NVT authentication password
        $mediauri   media web service uri
        $deviceuri  core web service uri
        $ptzuri     ptz web service uri
        $baseurl    url of the NVT (without service specification)
        $onvifversion   onvif version supported by the NVT
        $deltatime time differential correction (used to synchronize NVC with NVT)
        $capabilites    response of GetCapabilities
        $videosources   response of GetVideoSources
        $sources    array cotaining tokens for further requests
        $profiles response of GetProfiles
        $proxyhost  proxy
        $proxyport proxy port
        $proxyusername  proxy authentication username
        $proxypassword  proxy authentication password
        $lastresponse   last soap response
    */
    protected $ipaddress='';
    protected $username='';
    protected $password='';
    protected $mediauri='';
    protected $deviceuri='';
    protected $ptzuri='';
    protected $baseuri='';
    protected $onvifversion=array();
    protected $deltatime=0;
    protected $capabilities=array();
    protected $videosources=array();
    protected $sources=array();
    protected $profiles=array();
    protected $proxyhost='';
    protected $proxyport='';
    protected $proxyusername='';
    protected $proxypassword='';
    protected $lastresponse='';
    protected $intransingent=true;

    /*
        Properties wrappers
    */
    public function setProxyHost($proxyHost) { $this->proxyhost = $proxyHost; }
    public function getProxyHost() { return $this->proxyhost; }
    public function setProxyPort($proxyPort) { $this->proxyport = $proxyPort; }
    public function getProxyPort() { return $this->proxyport; }
    public function setProxyUsername($proxyUsername) { $this->proxyusername = $proxyUsername; }
    public function getProxyUsername() { return $this->proxyusername; }
    public function setProxyPassword($proxyPassword) { $this->proxypassword = $proxyPassword; }
    public function getProxyPassword() { return $this->proxypassword; }
    public function setUsername($username) { $this->username = $username; }
    public function getUsername() { return $this->username; }
    public function setPassword($password) { $this->password = $password; }
    public function getPassword() { return $this->password; }
    public function getDeviceUri() { return $this->mediauri; }
    public function setDeviceUri($deviceUri) { $this->mediauri=$deviceUrl; }
    public function getIPAddress($ipAddress) { return $this->ipAddress; }
    public function getSources() { return $this->sources; }
    public function getMediaUri() { return $this->mediauri; }
    public function getPTZUri() { return $this->ptzuri; }
    public function getBaseUrl() { return $this->baseurl; }
    public function getSupportedVersion() { return $this->onvifversion; }
    public function getCapabilities() { return $this->capabilities; }
    public function setBreakOnError($intransingent) { $this->intransingent=$intransingent; }
    public function getLastResponse() { return $this->lastresponse; }

    public function setIPAddress($ipAddress)
    {
        $this->ipaddress=$ipAddress;
        $this->mediauri='http://'.$this->ipaddress.'/onvif/device_service';
    }

    /*
        Constructor & Destructor
    */
    public function __construct()
    {
        // nothing to do
    }

    public function __destruct()
    {
        // nothing to do
    }

    /*
        Public functions (basic initialization method and other collaterals)
    */
    public function initialize() {
        $res = Array();
        $res['datetime'] = true;
        $res['profile'] = true;

        $this->mediauri='http://'.$this->ipaddress.'/onvif/device_service';

        try {
            $datetime=$this->core_GetSystemDateAndTime();

            if (empty($datetime)) {
                $res['datetime'] = false;
                return $res;
            }

            $timestamp=mktime($datetime['Time']['Hour'], $datetime['Time']['Minute'], $datetime['Time']['Second'],
                $datetime['Date']['Month'], $datetime['Date']['Day'], $datetime['Date']['Year']);
            $this->deltatime=time()-$timestamp-5;
        } catch (Exception $e) {
            throw new Exception('GetSystemDateAndTime: error');
        }

            $this->capabilities=$this->core_GetCapabilities();

        $onvifVersion=$this->_getOnvifVersion($this->capabilities);
        $this->mediauri=$onvifVersion['media'];
        $this->deviceuri=$onvifVersion['device'];
        $this->ptzuri=$onvifVersion['ptz'];
        preg_match("/^http(.*)onvif\//",$this->mediauri,$matches);
        $this->baseuri=$matches[0];
        $this->onvifversion=array(
            'major'=>$onvifVersion['major'],
            'minor'=>$onvifVersion['minor']
        );


        $this->videosources=$this->media_GetVideoSources();
        $this->profiles=$this->media_GetProfiles();
        if (empty($this->profiles)) {
            $res['profile'] = false;
            return $res;
        }

        $this->sources=$this->_getActiveSources($this->videosources,$this->profiles);

        return $res;
    }

    public function chOnvif($ip, $port = 80)
    {
        $res = false;

        $this->mediauri='http://'.$ip.':'.$port.'/onvif/device_service';
        try {
            $this->core_GetSystemDateAndTime(true);

            $res = true;
        } catch (Exception $e) {
        }

        return $res;
    }

    public function isFault($response) { // Useful to check if response contains a fault
        return array_key_exists('Fault', $response);
    }

    /*
        Public wrappers for a subset of ONVIF primitives
    */
    public function core_GetSystemDateAndTime($check = false, $auth = false) {
        if ($auth) {
            $REQ=$this->_makeToken();
            $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetSystemDateAndTime xmlns="http://www.onvif.org/ver10/device/wsdl"/></s:Body></s:Envelope>';
            $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP']),
                         $post_string);


        } else {
            $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetSystemDateAndTime xmlns="http://www.onvif.org/ver10/device/wsdl"/></s:Body></s:Envelope>';
        }

        if ($this->isFault($response=$this->_send_request($this->mediauri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetSystemDateAndTime: Communication error');
        }
        else {
            if ($check) {
                return $response['Envelope']['Body'];
            } else {
                if (isset($response['Envelope']['Body']['Fault'])) {
                    if (!$auth) return $this->core_GetSystemDateAndTime(false, true);
                } else {
                    if (isset($response['Envelope']['Body']['GetSystemDateAndTimeResponse']['SystemDateAndTime']['UTCDateTime'])) {
                        return $response['Envelope']['Body']['GetSystemDateAndTimeResponse']['SystemDateAndTime']['UTCDateTime'];
                    } else {
                        return '';
                    }
                }
            }
        }
    }

    public function core_GetCapabilities() {
        $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetCapabilities xmlns="http://www.onvif.org/ver10/device/wsdl"><Category>All</Category></GetCapabilities></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP']),
                         $post_string);
        if ($this->isFault($response=$this->_send_request($this->mediauri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetCapabilities: Communication error');
        }
        else
            return $response['Envelope']['Body']['GetCapabilitiesResponse']['Capabilities'];
    }

    public function media_GetVideoSources() {
        $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetVideoSources xmlns="http://www.onvif.org/ver10/media/wsdl"/></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP']),
                         $post_string);
        if ($this->isFault($response=$this->_send_request($this->mediauri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetVideoSources: Communication error');
        }
        else
            return $response['Envelope']['Body']['GetVideoSourcesResponse']['VideoSources'];
    }

    public function media_GetProfiles() {
        $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetProfiles xmlns="http://www.onvif.org/ver10/media/wsdl"/></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP']),
                         $post_string);
        if ($this->isFault($response=$this->_send_request($this->mediauri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetProfiles: Communication error');
        }
        else {
            if (isset($response['Envelope']['Body']['GetProfilesResponse']['Profiles'])) {
                return $response['Envelope']['Body']['GetProfilesResponse']['Profiles'];
            } else {
                return '';
            }
        }
    }

    public function media_GetServices() {
        $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope">

            <s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetServices xmlns="http://www.onvif.org/ver10/device/wsdl"><IncludeCapability>false</IncludeCapability></GetServices></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP']),
                         $post_string);
        if ($this->isFault($this->_send_request($this->mediauri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetServices: Communication error');
        }
    }

    public function core_GetDeviceInformation() {
        $REQ=$this->_makeToken();
        //$post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetDeviceInformation xmlns="http://www.onvif.org/ver10/device/wsdl"/></s:Body></s:Envelope>';

        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetDeviceInformation xmlns="http://www.onvif.org/ver10/device/wsdl"/></s:Body></s:Envelope>';

        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP']),
                         $post_string);
        if ($this->isFault($response=$this->_send_request($this->mediauri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetDeviceInformation: Communication error');
        }
        else
            return $response['Envelope']['Body']['GetDeviceInformationResponse'];
    }

    public function media_GetStreamUri($profileToken,$stream="RTP-Unicast",$protocol="RTSP") {
        $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetStreamUri xmlns="http://www.onvif.org/ver10/media/wsdl"><StreamSetup><Stream xmlns="http://www.onvif.org/ver10/schema">%%STREAM%%</Stream><Transport xmlns="http://www.onvif.org/ver10/schema"><Protocol>%%PROTOCOL%%</Protocol></Transport></StreamSetup><ProfileToken>%%PROFILETOKEN%%</ProfileToken></GetStreamUri></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%STREAM%%",
                   "%%PROTOCOL%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $stream,
                   $protocol),
                         $post_string);
        if ($this->isFault($response=$this->_send_request($this->mediauri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetStreamUri: Communication error');
        }
        else
            return $response['Envelope']['Body']['GetStreamUriResponse']['MediaUri']['Uri'];
    }

    public function getNetworkProtocols()
    {
        $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetNetworkProtocols xmlns="http://www.onvif.org/ver10/device/wsdl"/></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%STREAM%%",
                   "%%PROTOCOL%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP']
                           ),
                         $post_string);
        if ($this->isFault($response=$this->_send_request($this->mediauri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetStreamUri: Communication error');
        }
        else {
            if (isset($response['Envelope']['Body']['GetNetworkProtocolsResponse']['NetworkProtocols'])) {
                return $response['Envelope']['Body']['GetNetworkProtocolsResponse']['NetworkProtocols'];
            } else {
                return Array();
            }
        }
    }

    public function ptz_GetPresets($profileToken) {
        if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetPresets xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken></GetPresets></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken),
                         $post_string);
        if ($this->isFault($response=$this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetPresets: Communication error');
        }
        else {
            $getpresetsresponse=$response['Envelope']['Body']['GetPresetsResponse']['Preset'];
            $presets=array();
            foreach ($getpresetsresponse as $preset) {
                    $presets[]=array('Token'=>$preset['@attributes']['token'],
                                 'Name'=>$preset['Name'],
                                 'PTZPosition'=>$preset['PTZPosition']);
            }
            return $presets;
        }
    }

    public function ptz_GetNode($ptzNodeToken) {
        if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GetNode xmlns="http://www.onvif.org/ver20/ptz/wsdl"><NodeToken>%%NODETOKEN%%</NodeToken></GetNode></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%NODETOKEN%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $ptzNodeToken),
                         $post_string);
        if ($this->isFault($response=$this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('GetNode: Communication error');
        }
        else
            return $response['Envelope']['Body']['GetNodeResponse'];
    }

    public function ptz_GotoPreset($profileToken,$presetToken,$speed_pantilt_x,$speed_pantilt_y,$speed_zoom_x) {
        if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><GotoPreset xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken><PresetToken>%%PRESETTOKEN%%</PresetToken><Speed><PanTilt x="%%SPEEDPANTILTX%%" y="%%SPEEDPANTILTY%%" xmlns="http://www.onvif.org/ver10/schema"/><Zoom x="%%SPEEDZOOMX%%" xmlns="http://www.onvif.org/ver10/schema"/></Speed></GotoPreset></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%PRESETTOKEN%%",
                   "%%SPEEDPANTILTX%%",
                   "%%SPEEDPANTILTY%%",
                   "%%SPEEDZOOMX%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $presetToken,
                   $speed_pantilt_x,
                   $speed_pantilt_y,
                   $speed_zoom_x),
                         $post_string);
        if ($this->isFault($this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('GotoPreset: Communication error');
        }
    }

    public function ptz_RemovePreset($profileToken,$presetToken) {
            if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><RemovePreset xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken><PresetToken>%%PRESETTOKEN%%</PresetToken></RemovePreset></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%PRESETTOKEN%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $presetToken),
                         $post_string);
        if ($this->isFault($this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('RemovePreset: Communication error');
        }
    }

    public function ptz_SetPreset($profileToken,$presetName) {
            if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
        $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><SetPreset xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken><PresetName>%%PRESETNAME%%</PresetName></SetPreset></s:Body></s:Envelope>';
        $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%PRESETNAME%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $presetName),
                         $post_string);
        if ($this->isFault($this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('SetPreset: Communication error');
        }
    }

    public function ptz_RelativeMove($profileToken,$translation_pantilt_x,$translation_pantilt_y,$speed_pantilt_x,$speed_pantilt_y) {
        if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
            $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><RelativeMove xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken><Translation><PanTilt x="%%TRANSLATIONPANTILTX%%" y="%%TRANSLATIONPANTILTY%%" space="http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace" xmlns="http://www.onvif.org/ver10/schema"/></Translation><Speed><PanTilt x="%%SPEEDPANTILTX%%" y="%%SPEEDPANTILTY%%" space="http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace" xmlns="http://www.onvif.org/ver10/schema"/></Speed></RelativeMove></s:Body></s:Envelope>';
            $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%TRANSLATIONPANTILTX%%",
                   "%%TRANSLATIONPANTILTY%%",
                   "%%SPEEDPANTILTX%%",
                   "%%SPEEDPANTILTY%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $translation_pantilt_x,
                   $translation_pantilt_y,
                   $speed_pantilt_x,
                   $speed_pantilt_y),
                         $post_string);
        if ($this->isFault($this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('RelativeMove: Communication error');
        }
    }

    public function ptz_RelativeMoveZoom($profileToken,$zoom,$speedZoom) {
        if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
            $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><RelativeMove xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken><Translation><Zoom x="%%ZOOM%%" space="http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace" xmlns="http://www.onvif.org/ver10/schema"/></Translation><Speed><Zoom x="%%SPEEDZOOM%%" space="http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace" xmlns="http://www.onvif.org/ver10/schema"/></Speed></RelativeMove></s:Body></s:Envelope>';
            $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%SPEEDZOOM%%",
                   "%%ZOOM%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $speedZoom,
                   $zoom),
                         $post_string);
                if ($this->isFault($this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('RelativeMoveZoom: Communication error');
        }
    }

    public function ptz_AbsoluteMove($profileToken,$position_pantilt_x,$position_pantilt_y,$zoom) {
        if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
            $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><AbsoluteMove xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken><Position><PanTilt x="%%POSITIONPANTILTX%%" y="%%POSITIONPANTILTY%%" space="http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace" xmlns="http://www.onvif.org/ver10/schema"/><Zoom x="%%ZOOM%%" space="http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace" xmlns="http://www.onvif.org/ver10/schema"/></Position></AbsoluteMove></s:Body></s:Envelope>';
            $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%POSITIONPANTILTX%%",
                   "%%POSITIONPANTILTY%%",
                   "%%ZOOM%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $position_pantilt_x,
                   $position_pantilt_y,
                   $zoom),
                         $post_string);
        if ($this->isFault($this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('AbsoluteMove: Communication error');
        }
    }

    public function ptz_ContinuousMove($profileToken,$velocity_pantilt_x,$velocity_pantilt_y) {
        if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
            $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><ContinuousMove xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken><Velocity><PanTilt x="%%VELOCITYPANTILTX%%" y="%%VELOCITYPANTILTY%%" space="http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace" xmlns="http://www.onvif.org/ver10/schema"/></Velocity></ContinuousMove></s:Body></s:Envelope>';
            $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%VELOCITYPANTILTX%%",
                   "%%VELOCITYPANTILTY%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $velocity_pantilt_x,
                   $velocity_pantilt_y),
                         $post_string);
        if ($this->isFault($this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('ContinuousMove: Communication error');
        }
    }

    public function ptz_ContinuousMoveZoom($profileToken,$zoom) {
        if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
            $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><ContinuousMove xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken><Velocity><Zoom x="%%ZOOM%%" space="http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace" xmlns="http://www.onvif.org/ver10/schema"/></Velocity></ContinuousMove></s:Body></s:Envelope>';
            $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%ZOOM%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $zoom),
                         $post_string);
        if ($this->isFault($this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('ContinuousMoveZoom: Communication error');
        }
    }

    public function ptz_Stop($profileToken,$pantilt,$zoom) {
        if ($this->ptzuri=='') return array();
            $REQ=$this->_makeToken();
            $post_string='<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope"><s:Header><Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"><UsernameToken><Username>%%USERNAME%%</Username><Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">%%PASSWORD%%</Password><Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">%%NONCE%%</Nonce><Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">%%CREATED%%</Created></UsernameToken></Security></s:Header><s:Body xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"><Stop xmlns="http://www.onvif.org/ver20/ptz/wsdl"><ProfileToken>%%PROFILETOKEN%%</ProfileToken><PanTilt>%%PANTILT%%</PanTilt><Zoom>%%ZOOM%%</Zoom></Stop></s:Body></s:Envelope>';
            $post_string=str_replace(array("%%USERNAME%%",
                               "%%PASSWORD%%",
                               "%%NONCE%%",
                               "%%CREATED%%",
                   "%%PROFILETOKEN%%",
                   "%%PANTILT%%",
                   "%%ZOOM%%"),
                         array($REQ['USERNAME'],
                               $REQ['PASSDIGEST'],
                               $REQ['NONCE'],
                               $REQ['TIMESTAMP'],
                   $profileToken,
                   $pantilt,
                   $zoom),
                         $post_string);
        if ($this->isFault($this->_send_request($this->ptzuri,$post_string))) {
            if ($this->intransingent) throw new Exception('Stop: Communication error');
        }
    }

    /*
        Internal functions
    */
    protected function _makeToken() {
        $timestamp=time()-$this->deltatime;
        return $this->_passwordDigest($this->username,$this->password,date('Y-m-d\TH:i:s.000\Z',$timestamp));
    }

    protected function _getOnvifVersion($capabilities) {
        $version=array();
        if (isset($capabilities['Device']['System']['SupportedVersions']['Major'])) {
        // NVT supports a specific onvif version
        $version['major']=$capabilities['Device']['System']['SupportedVersions']['Major'];
        $version['minor']=$capabilities['Device']['System']['SupportedVersions']['Minor'];
        } else {
        // NVT supports more onvif versions
        $currentma=0;
        $currentmi=0;
        foreach ($capabilities['Device']['System']['SupportedVersions'] as $cver) {
            if ($cver['Major']>$currentma) { $currentma=$cver['Major'];$currentmi=$cver['Minor']; }
        }
        $version['major']=$currentma;
        $version['minor']=$currentmi;
        }
        $version['media']=$capabilities['Media']['XAddr'];
        $version['device']=$capabilities['Device']['XAddr'];
        $version['event']=$capabilities['Events']['XAddr'];
        if (isset($capabilities['PTZ']['XAddr'])) $version['ptz']=$capabilities['PTZ']['XAddr']; else $version['ptz']='';
        return $version;
    }

    protected function _getActiveSources($videoSources,$profiles) {
      $sources=array();

      if (isset($videoSources['@attributes'])) {
        // NVT is a camera
            $sources[0]['sourcetoken']=$videoSources['@attributes']['token'];
        $this->_getProfileData($sources,0,$profiles);
      }
      else {
           // NVT is an encoder
           for ($i=0;$i<count($videoSources);$i++) {
                if (strtolower($videoSources[$i]['@attributes']['SignalActive'])=='true') {
                        $sources[$i]['sourcetoken']=$videoSources[$i]['@attributes']['token'];
                $this->_getProfileData($sources,$i,$profiles);
               }
           } // for
      }

      return $sources;
    }

    protected function _getProfileData(&$sources,$i,$profiles) {
        $inprofile=0;
        for ($y=0;$y<count($profiles);$y++) {
            if ($profiles[$y]['VideoSourceConfiguration']['SourceToken']==$sources[$i]['sourcetoken']) {
                $sources[$i][$inprofile]['profiletoken']=$profiles[$y]['@attributes']['token'];
                $sources[$i][$inprofile]['profiletoken_name']=$profiles[$y]['Name'];
                $sources[$i][$inprofile]['encoding']=$profiles[$y]['VideoEncoderConfiguration']['Encoding'];
                $sources[$i][$inprofile]['width']=$profiles[$y]['VideoEncoderConfiguration']['Resolution']['Width'];
                $sources[$i][$inprofile]['height']=$profiles[$y]['VideoEncoderConfiguration']['Resolution']['Height'];
                $sources[$i][$inprofile]['fps']=$profiles[$y]['VideoEncoderConfiguration']['RateControl']['FrameRateLimit'];
                $sources[$i][$inprofile]['bitrate']=$profiles[$y]['VideoEncoderConfiguration']['RateControl']['BitrateLimit'];
                if ( isset($profiles[$y]['PTZConfiguration'])) {
                        $sources[$i][$inprofile]['ptz']['name']=$profiles[$y]['PTZConfiguration']['Name'];
                        $sources[$i][$inprofile]['ptz']['nodetoken']=$profiles[$y]['PTZConfiguration']['NodeToken'];
                }
                $inprofile++;
            }
        }
    }

    protected function _xml2array($response) {
        $sxe = new SimpleXMLElement($response, LIBXML_NOERROR);
        $dom_sxe = dom_import_simplexml($sxe);
        $dom = new DOMDocument('1.0');
        $dom_sxe = $dom->importNode($dom_sxe, true);
        $dom_sxe = $dom->appendChild($dom_sxe);
        $element = $dom->childNodes->item(0);
        foreach ($sxe->getDocNamespaces() as $name => $uri) {
            $element->removeAttributeNS($uri, $name);
        }
        $xmldata=$dom->saveXML();
        $xmldata=substr($xmldata,strpos($xmldata,"<Envelope>"));
        $xmldata=substr($xmldata,0,strpos($xmldata,"</Envelope>")+strlen("</Envelope>"));
        $xml=simplexml_load_string($xmldata);
        $data=json_decode(json_encode((array)$xml),1);
        $data=array($xml->getName()=>$data);
        return $data;
    }

    protected function _passwordDigest( $username, $password, $timestamp = "default", $nonce = "default" ) {
        if ($timestamp=='default') $timestamp=date('Y-m-d\TH:i:s.000\Z');
        if ($nonce=='default') $nonce=mt_rand();
        $REQ=array();
        $passdigest = base64_encode(pack('H*', sha1(pack('H*', $nonce) . pack('a*',$timestamp).pack('a*',$password))));
        //$passdigest=base64_encode(sha1($nonce.$timestamp.$password,true)); // alternative
        $REQ['USERNAME']=$username;
        $REQ['PASSDIGEST']=$passdigest;
        $REQ['NONCE']=base64_encode(pack('H*', $nonce));
        //$REQ['NONCE']=base64_encode($nonce); // alternative
        $REQ['TIMESTAMP']=$timestamp;
        return $REQ;
    }

    protected function _send_request($url,$post_string) {
        $soap_do = curl_init();
        curl_setopt($soap_do, CURLOPT_URL,            $url );
        if ($this->proxyhost!='' && $this->proxyport!='') {
          curl_setopt($soap_do, CURLOPT_PROXY, 	      $this->proxyhost);
          curl_setopt($soap_do, CURLOPT_PROXYPORT,    $this->proxyport);
          if ($this->proxyusername!='' && $this->proxypassword!='')
            curl_setopt($soap_do, CURLOPT_PROXYUSERPWD, $this->proxyusername.':'.$this->proxypassword);
        }
        curl_setopt($soap_do, CURLOPT_CONNECTTIMEOUT, 10);
        curl_setopt($soap_do, CURLOPT_TIMEOUT,        10);
        curl_setopt($soap_do, CURLOPT_RETURNTRANSFER, true );
        curl_setopt($soap_do, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($soap_do, CURLOPT_SSL_VERIFYHOST, false);
        curl_setopt($soap_do, CURLOPT_POST,           true );
        curl_setopt($soap_do, CURLOPT_POSTFIELDS,    $post_string);
        curl_setopt($soap_do, CURLOPT_HTTPHEADER,     array('Content-Type: text/xml; charset=utf-8', 'Content-Length: '.strlen($post_string) ));
        //curl_setopt($soap_do, CURLOPT_USERPWD, $user . ":" . $password); // HTTP authentication
        if ( ($result = curl_exec($soap_do)) === false) {
            $err = curl_error($soap_do);
            $this->lastresponse=array("Fault"=>$err);
        } else {
            $this->lastresponse=$this->_xml2array($result);
        }
        return $this->lastresponse;
    }
} // end class

