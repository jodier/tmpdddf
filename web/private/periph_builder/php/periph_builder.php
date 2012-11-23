<?php
/*-------------------------------------------------------------------------*/
/* CLASS TDBBuilder							   */
/*-------------------------------------------------------------------------*/

require_once('db_builder.php');

/*-------------------------------------------------------------------------*/
/* CLASS TConnectionBuilder						   */
/*-------------------------------------------------------------------------*/

class TConnectionBuilder extends TDBBuilder
{
	/*-----------------------------------------------------------------*/

	function Remove($options)
	{
		/* TEMP */
		$this->Redirect();
		/* TEMP */

		TDBBuilder::Remove($options, false);

		try
		{
			$query_string = 'DROP DATABASE %DBNAME%;';

			$query_string = str_replace('%DBNAME%', 'periph_builder_content_' . $options['name'], $query_string);

			$pdo = new PDO('mysql:host='.$options['url'].';', $options['login'], $options['password']);
			$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
			$pdo->prepare($query_string)->execute();
		}
 		catch(Exception $e)
		{
			$m = $e->getMessage();

			if(strstr($m, 'HY000') == false)
			{
				print("<html>\n");
				print("  <p>$query_string;</p>\n");
				print("  <p style=\"color: red;\">$m</p>\n");
				print("</html>");

				exit(1);
			}
		}

		$this->Redirect();
	}

	/*-----------------------------------------------------------------*/

	function Append($options)
	{
		TDBBuilder::Append($options, false);

		try
		{
			$query_string = file_get_contents('http://localhost/private/periph_builder/php/periph.sql');

			$query_string = str_replace('%DBNAME%', 'periph_builder_content_' . $options['name'], $query_string);

			$pdo = new PDO('mysql:host='.$options['url'].';', $options['login'], $options['password']);
			$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
			$pdo->prepare($query_string)->execute();
		}
 		catch(Exception $e)
		{
			$m = $e->getMessage();

			if(strstr($m, 'HY000') == false)
			{
				print("<html>\n");
				print("  <p>$query_string;</p>\n");
				print("  <p style=\"color: red;\">$m</p>\n");
				print("</html>");

				exit(1);
			}
		}

		$this->Redirect();
	}

	/*-----------------------------------------------------------------*/
}

/*-------------------------------------------------------------------------*/
/* CLASS TArchBuilder							   */
/*-------------------------------------------------------------------------*/

class TArchBuilder extends TDBBuilder
{
	/*-----------------------------------------------------------------*/

	function Sort($table)
	{
		$L = array(
			'device' => ' ORDER BY name',
			'region' => ' ORDER BY base',
			'base' => ' ORDER BY region, offset',
			'base_mult' => ' ORDER BY base, offset',
			'register' => ' ORDER BY device, base, offset',
			'register_mult' => ' ORDER BY register, offset',
			'field' => ' ORDER BY register, shift',
			'field_mult' => ' ORDER BY field, shift',
			'enum' => ' ORDER BY field',
		);

		return $L[$table];
	}

	/*-----------------------------------------------------------------*/

	function Coord($table)
	{
		$L = array(
			'device' => '132,16,224,48',
			'region' => '1,82,92,127',
			'base' => '132,115,224,186',
			'base_mult' => '0,167,92,226',
			'register' => '271,16,362,114',
			'register_mult' => '271,160,362,219',
			'field' => '389,16,481,114',
			'field_mult' => '389,154,481,213',
			'enum' => '508,36,599,94',
		);

		return $L[$table];
	}

	/*-----------------------------------------------------------------*/

	function Patch($name, $value)
	{
		/**/ if($name == 'base' || ($name == 'offset' && $this->table != 'field'))
		{
			$value = strtoupper(dechex($value));

			$value = "0x$value";
		}
		else if($name == 'reset_value')
		{
			$value = strtoupper(decbin($value));

			$value = "0b$value";
		}

		return $value;
	}

	/*-----------------------------------------------------------------*/
}

/*-------------------------------------------------------------------------*/

$db = isset($_GET['db']) ? $_GET['db'] : 'periph_builder_config';

if($_SERVER['HTTP_HOST'] != 'localhost')
{
	$login = isset($_GET['login']) ? $_GET['login'] : 'unknown';

	$password = isset($_GET['password']) ? $_GET['password'] : 'unknown';
}
else
{
	$login = 'root';

	$password = 'root';
}

/*-------------------------------------------------------------------------*/

if($db == 'periph_builder_config')
{
	/*-----------------------------------------------------------------*/
	/* CONNECTION							   */
	/*-----------------------------------------------------------------*/

	$table = isset($_GET['table']) ? $_GET['table'] : 'periph_list';

	$ConnectionBuilder = new TConnectionBuilder('localhost', $db, 'root', 'root', $table);

	/*-----------------------------------------------------------------*/

	/**/ if(isset($_GET['remove']))
	{
		$ConnectionBuilder->Remove($_GET);
	}
	else if(isset($_GET['append']))
	{
		$ConnectionBuilder->Append($_GET);
	}
	else if(isset($_GET['update']))
	{
		$ConnectionBuilder->Update($_GET);
	}
	else if(isset($_GET[ 'json' ]))
	{
		$ConnectionBuilder->EmitJSONContent($_GET['id']);
	}

	/*-----------------------------------------------------------------*/
}
else
{
	/*-----------------------------------------------------------------*/
	/* APPLICATION							   */
	/*-----------------------------------------------------------------*/

	$table = isset($_GET['table']) ? $_GET['table'] : 'device';

	$ArchBuilder = new TArchBuilder('localhost', $db, $login, $password, $table);

	/*-----------------------------------------------------------------*/

	/**/ if(isset($_GET['remove']))
	{
		$ArchBuilder->Remove($_GET);
	}
	else if(isset($_GET['append']))
	{
		$ArchBuilder->Append($_GET);
	}
	else if(isset($_GET['update']))
	{
		$ArchBuilder->Update($_GET);
	}
	else if(isset($_GET[ 'json' ]))
	{
		$ArchBuilder->EmitJSONContent($_GET['id']);
	}

	/*-----------------------------------------------------------------*/
}

/*-------------------------------------------------------------------------*/

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

/*-------------------------------------------------------------------------*/
?>