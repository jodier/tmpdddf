<?php
/**
 * Tine 2.0
 * 
 * @package     Timetracker
 * @subpackage  Controller
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2010 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

/**
 * Timetracker Controller
 * 
 * @package     Timetracker
 * @subpackage  Controller
 */
class Timetracker_Controller extends Tinebase_Controller_Abstract
{
    /**
     * application name
     *
     * @var string
     */
    protected $_applicationName = 'Timetracker';
    
    /**
     * holds the default Model of this application
     * @var string
     */
    protected static $_defaultModel = 'Timetracker_Model_Timeaccount';
    
    /**
     * the constructor
     *
     * don't use the constructor. use the singleton 
     */
    private function __construct() {
    }
    
    /**
     * don't clone. Use the singleton.
     *
     */
    private function __clone() 
    {
    }
    
    /**
     * holds self
     * @var Timetracker_Controller
     */
    private static $_instance = NULL;
    
    /**
     * singleton
     *
     * @return Timetracker_Controller
     */
    public static function getInstance() 
    {
        if (self::$_instance === NULL) {
            self::$_instance = new Timetracker_Controller();
        }
        return self::$_instance;
    }
}
