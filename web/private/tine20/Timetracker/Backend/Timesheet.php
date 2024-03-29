<?php
/**
 * Tine 2.0
 *
 * @package     Timetracker
 * @subpackage  Backend
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */

/**
 * backend for timesheets
 *
 * @package     Timetracker
 * @subpackage  Backend
 */
class Timetracker_Backend_Timesheet extends Tinebase_Backend_Sql_Abstract
{
    /**
     * Table name without prefix
     *
     * @var string
     */
    protected $_tableName = 'timetracker_timesheet';
    
    /**
     * Model name
     *
     * @var string
     */
    protected $_modelName = 'Timetracker_Model_Timesheet';

    /**
     * if modlog is active, we add 'is_deleted = 0' to select object in _getSelect()
     *
     * @var boolean
     */
    protected $_modlogActive = TRUE;
    
    /**
     * default secondary sort criteria
     * 
     * @var string
     */
    protected $_defaultSecondarySort = 'timetracker_timesheet.creation_time';
    
    /**
     * additional search count columns
     * 
     * @var array
     */
    protected $_additionalSearchCountCols =  array(
        'count'                => 'COUNT(*)', 
        'countBillable'        => 'SUM(is_billable_combined)',
        'sum'                  => 'SUM(duration)',
        'sumBillable'          => 'SUM(duration*is_billable_combined)',
    );
    
    /**
     * foreign tables (key => tablename)
     *
     * @var array
     */
    protected $_foreignTables = array(
        'is_billable_combined'    => array(
            'table'         => 'timetracker_timeaccount',
            'joinOn'        => 'id',
            'joinId'        => 'timeaccount_id',
            'select'        => array('is_billable_combined' => '(timetracker_timesheet.is_billable * timetracker_timeaccount.is_billable)'),
            'singleValue'   => TRUE,
            // needs to be preserved in select
            'preserve'      => TRUE,
        ),
        'is_cleared_combined'    => array(
            'table'         => 'timetracker_timeaccount',
            'joinOn'        => 'id',
            'joinId'        => 'timeaccount_id',
            'select'        => array('is_cleared_combined'  => "( (CASE WHEN timetracker_timesheet.is_cleared = '1' THEN 1 ELSE 0 END) | (CASE WHEN timetracker_timeaccount.status = 'billed' THEN 1 ELSE 0 END) )"),
            'singleValue'   => TRUE,
            // needs to be preserved in select
            'preserve'      => TRUE,
        ),
    );
    
    /**
     * the constructor
     *
     * allowed options:
     *  - modelName
     *  - tableName
     *  - tablePrefix
     *  - modlogActive
     *  - useSubselectForCount
     *
     * @param Zend_Db_Adapter_Abstract $_db (optional)
     * @param array $_options (optional)
     * @throws Tinebase_Exception_Backend_Database
     */
    public function __construct($_dbAdapter = NULL, $_options = array())
    {
        parent::__construct($_dbAdapter, $_options);
        
        // convert to Zend_Db_Expr()
        $this->_foreignTables['is_billable_combined']['select']['is_billable_combined'] = 
            new Zend_Db_Expr($this->_foreignTables['is_billable_combined']['select']['is_billable_combined']);
        
        $this->_foreignTables['is_cleared_combined']['select']['is_cleared_combined']   = 
            new Zend_Db_Expr($this->_foreignTables['is_cleared_combined']['select']['is_cleared_combined']);
    }
}
