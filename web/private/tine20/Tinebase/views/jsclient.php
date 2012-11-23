<?php
/**
 * Tine 2.0 main view
 * 
 * @package     Tinebase
 * @subpackage  Views
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Lars Kneschke <l.kneschke@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
    <title>Tine 2.0</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=8; IE=7" />
    <meta http-equiv="X-Tine20-Version" content="<?php echo TINE20_PACKAGESTRING ?>" />
    
    <link rel="icon" href="images/favicon.ico" type="image/x-icon" />
    <link rel="chrome-application-definition" href="chrome_web_app.json" />
</head>
<body>
    <!-- Loading Indicator -->
    <div class="tine-viewport-waitcycle" style="position: absolute; top: 50%; left: 50%; background-image: url(data:image/gif;base64,R0lGODlhEAAQALMMAKqooJGOhp2bk7e1rZ2bkre1rJCPhqqon8PBudDOxXd1bISCef///wAAAAAAAAAAACH/C05FVFNDQVBFMi4wAwEAAAAh+QQFAAAMACwAAAAAEAAQAAAET5DJyYyhmAZ7sxQEs1nMsmACGJKmSaVEOLXnK1PuBADepCiMg/DQ+/2GRI8RKOxJfpTCIJNIYArS6aRajWYZCASDa41Ow+Fx2YMWOyfpTAQAIfkEBQAADAAsAAAAABAAEAAABE6QyckEoZgKe7MEQMUxhoEd6FFdQWlOqTq15SlT9VQM3rQsjMKO5/n9hANixgjc9SQ/CgKRUSgw0ynFapVmGYkEg3v1gsPibg8tfk7CnggAIfkEBQAADAAsAAAAABAAEAAABE2QycnOoZjaA/IsRWV1goCBoMiUJTW8A0XMBPZmM4Ug3hQEjN2uZygahDyP0RBMEpmTRCKzWGCkUkq1SsFOFQrG1tr9gsPc3jnco4A9EQAh+QQFAAAMACwAAAAAEAAQAAAETpDJyUqhmFqbJ0LMIA7McWDfF5LmAVApOLUvLFMmlSTdJAiM3a73+wl5HYKSEET2lBSFIhMIYKRSimFriGIZiwWD2/WCw+Jt7xxeU9qZCAAh+QQFAAAMACwAAAAAEAAQAAAETZDJyRCimFqbZ0rVxgwF9n3hSJbeSQ2rCWIkpSjddBzMfee7nQ/XCfJ+OQYAQFksMgQBxumkEKLSCfVpMDCugqyW2w18xZmuwZycdDsRACH5BAUAAAwALAAAAAAQABAAAARNkMnJUqKYWpunUtXGIAj2feFIlt5JrWybkdSydNNQMLaND7pC79YBFnY+HENHMRgyhwPGaQhQotGm00oQMLBSLYPQ9QIASrLAq5x0OxEAIfkEBQAADAAsAAAAABAAEAAABE2QycmUopham+da1cYkCfZ94UiW3kmtbJuRlGF0E4Iwto3rut6tA9wFAjiJjkIgZAYDTLNJgUIpgqyAcTgwCuACJssAdL3gpLmbpLAzEQA7); width: 16px; height: 16px">&#160;</div><div class="tine-viewport-poweredby" style="position: absolute; bottom: 10px; right: 10px; font:normal 12px arial, helvetica,tahoma,sans-serif;">Powered by: <a target="_blank" href="http://www.tine20.org/">Tine 2.0</a></div>
    
    <!-- EXT JS -->
     <?php
        if(isset(Tinebase_Core::getConfig()->themes->default)) {
            $path = Tinebase_Core::getConfig()->themes->themelist->get(Tinebase_Core::getConfig()->themes->default)->path;
            if(1 || Tinebase_Core::getConfig()->themes->themelist->get(Tinebase_Core::getConfig()->themes->default)->useBlueAsBase==1) {
                echo '<link rel="stylesheet" type="text/css" href="library/ExtJS/resources/css/ext-all.css" />';
            } else {
                echo '<link rel="stylesheet" type="text/css" href="library/ExtJS/resources/css/ext-all-notheme.css" />';
            }
            echo "\n".'<link rel="stylesheet" type="text/css" href="'.($path).'" />';
            echo "\n";
        } else {
            echo '<link rel="stylesheet" type="text/css" href="library/ExtJS/resources/css/ext-all.css" />';
            echo '<link rel="stylesheet" type="text/css" href="styles/tine20.css" />';
        }
     ?>
    
    <script type="text/javascript" src="library/ExtJS/adapter/ext/ext-base<?php echo TINE20_BUILDTYPE != 'RELEASE' ? '-debug' : '' ?>.js"></script>
    <script type="text/javascript" src="library/ExtJS/ext-all<?php echo TINE20_BUILDTYPE != 'RELEASE' ? '-debug' : '' ?>.js"></script>
    
    <?php require 'Tinebase/views/includeJsAndCss.php'; ?>
        
    <noscript><p>You need to enable javascript to use <a href="http://www.tine20.org/" title="online open source groupware and crm">Tine 2.0</a></p></noscript>
</body>
</html>
