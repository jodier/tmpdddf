<?php
/*-------------------------------------------------------------------------*/
/* UTILS								   */
/*-------------------------------------------------------------------------*/

function array_lookup_and_push(&$array, $item)
{
	$result = false;

	foreach($array as $ITEM)
	{
		if($ITEM == $item)
		{
			$result = true;

			break;
		}
	}

	if($result == false)
	{
		array_push($array, $item);
	}

	return $result;
}

/*-------------------------------------------------------------------------*/
/* CLASS TDBBuilder							   */
/*-------------------------------------------------------------------------*/

class TDBBuilder
{
	/*-----------------------------------------------------------------*/

	function __construct($host, $db, $login, $password, $table)
	{
		/*---------------------------------------------------------*/
		/* CONNECTION						   */
		/*---------------------------------------------------------*/

		try
		{
			$this->db_table /*--------*/ = new PDO("mysql:host=$host;dbname=$db" /*----------*/, $login, $password);
			$this->db_table /*--------*/->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

			$this->db_information_schema = new PDO("mysql:host=$host;dbname=information_schema", $login, $password);
			$this->db_information_schema->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

			$this->db = $db;
			$this->login = $login;
			$this->password = $password;
			$this->table = $table;

		}
 		catch(Exception $e)
		{
			$m = $e->getMessage();

			if(strstr($m, 'HY000') == false)
			{
				print("<html>\n");
				print("  <p style=\"color: red;\">$m</p>\n");
				print("</html>");

				exit(1);
			}
		}

		/*---------------------------------------------------------*/
		/* FILTER						   */
		/*---------------------------------------------------------*/

		if(isset($_GET['parent_table'])
		   &&
		   isset($_GET['parent_id'   ])
		 ) {
			$nameList = $this->SQLFieldNameList();

			$this->parent_table = $_GET['parent_table'];
			$this->parent_id = $_GET['parent_id'];

			foreach($nameList as $name)
			{
				$PARENT_NAME = $this->SQLLinkedTable($name);

				if($PARENT_NAME == $this->parent_table)
				{
					$this->filter1 = "WHERE $name = '$this->parent_id'";

					$this->XXXX = $name;

					break;
				}
			}

			$this->filter2 = "WHERE id = '$this->parent_id'";
		}
		else
		{
			$this->parent_table = '';
			$this->parent_id = '';

			$this->filter1 = '';
			$this->filter2 = '';

			$this->XXXX = '';
		}

		/*---------------------------------------------------------*/
		/* SORT							   */
		/*---------------------------------------------------------*/

		$this->sort = $this->Sort($table);

		/*---------------------------------------------------------*/
	}

	/*-----------------------------------------------------------------*/

	function __destruct() { }

	/*-----------------------------------------------------------------*/

	function SQLQuery($query_string)
	{
		$result = array();

		try
		{
			$query = $this->db_table->query($query_string);

			$result = $query->fetchAll(PDO::FETCH_ASSOC);

			$query->closeCursor();
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

		return $result;
	}

	/*-----------------------------------------------------------------*/

	function SQLFieldNameList()
	{
		$result = array();

		try
		{
			/*-------------------------------------------------*/
			/* SQL QUERY					   */
			/*-------------------------------------------------*/

			$query_string = "SHOW FIELDS FROM $this->table";

			$query = $this->db_table->query($query_string);

			$lines = $query->fetchAll(PDO::FETCH_ASSOC);

			$query->closeCursor();

			/*-------------------------------------------------*/
			/* NAME LIST					   */
			/*-------------------------------------------------*/

			foreach($lines as $line)
			{
				if($line['Field'] != 'id')
				{
					array_push($result, $line['Field']);
				}
			}

			/*-------------------------------------------------*/
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

		return $result;
	}

	/*-----------------------------------------------------------------*/

	function SQLFieldTypeList()
	{
		$result = array();

		try
		{
			/*-------------------------------------------------*/
			/* SQL QUERY					   */
			/*-------------------------------------------------*/

			$query_string = "SHOW FIELDS FROM $this->table";

			$query = $this->db_table->query($query_string);

			$lines = $query->fetchAll(PDO::FETCH_ASSOC);

			$query->closeCursor();

			/*-------------------------------------------------*/
			/* TYPE LIST					   */
			/*-------------------------------------------------*/

			$result = array();

			foreach($lines as $line)
			{
				if($line['Field'] != 'id')
				{
					array_push($result, $line['Type']);
				}
			}

			/*-------------------------------------------------*/
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

		return $result;
	}

	/*-----------------------------------------------------------------*/

	function SQLLinkedTable($name)
	{
		$result = '';

		try
		{
			/*-------------------------------------------------*/
			/* SQL QUERY					   */
			/*-------------------------------------------------*/

			$query_string = "SELECT CONSTRAINT_NAME, TABLE_NAME, COLUMN_NAME, REFERENCED_TABLE_NAME, REFERENCED_COLUMN_NAME"
				      . " FROM KEY_COLUMN_USAGE"
				      . " WHERE TABLE_NAME = '$this->table' AND COLUMN_NAME = '$name'"
			;

			$query = $this->db_information_schema->query($query_string);

			$lines = $query->fetchAll(PDO::FETCH_ASSOC);

			$query->closeCursor();

			/*-------------------------------------------------*/

			foreach($lines as $line)
			{
				if(strncmp($line['CONSTRAINT_NAME'], 'fk_', 3) == 0)
				{
					$result = $line['REFERENCED_TABLE_NAME'];

					break;
				}
			}

			/*-------------------------------------------------*/
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

		return $result;
	}

	/*-----------------------------------------------------------------*/

	function SQLParentTableList($table)
	{
		$result = array();

		try
		{
			/*-------------------------------------------------*/
			/* SQL QUERY					   */
			/*-------------------------------------------------*/

			$query_string = "SELECT CONSTRAINT_NAME, TABLE_NAME, COLUMN_NAME, REFERENCED_TABLE_NAME, REFERENCED_COLUMN_NAME"
				      . " FROM KEY_COLUMN_USAGE"
				      . " WHERE TABLE_NAME = '$table'"
			;

			$query = $this->db_information_schema->query($query_string);

			$lines = $query->fetchAll(PDO::FETCH_ASSOC);

			$query->closeCursor();

			/*-------------------------------------------------*/
			/* TYPE LIST					   */
			/*-------------------------------------------------*/

			foreach($lines as $line)
			{
				if(strncmp($line['CONSTRAINT_NAME'], 'fk_', 3) == 0)
				{
					array_push($result, array(
						'TABLE_NAME' => $line['TABLE_NAME'],
						'COLUMN_NAME' => $line['COLUMN_NAME'],
						'REFERENCED_TABLE_NAME' => $line['REFERENCED_TABLE_NAME'],
						'REFERENCED_COLUMN_NAME' => $line['REFERENCED_COLUMN_NAME'],
					));
				}
			}

			/*-------------------------------------------------*/
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

		return $result;
	}

	/*-----------------------------------------------------------------*/

	function SQLRelatedTableList($table)
	{
		$result = array();

		try
		{
			/*-------------------------------------------------*/
			/* SQL QUERY					   */
			/*-------------------------------------------------*/

			$query_string = "SELECT CONSTRAINT_NAME, TABLE_NAME, COLUMN_NAME, REFERENCED_TABLE_NAME, REFERENCED_COLUMN_NAME"
				      . " FROM KEY_COLUMN_USAGE"
				      . " WHERE REFERENCED_TABLE_NAME = '$table'"
			;

			$query = $this->db_information_schema->query($query_string);

			$lines = $query->fetchAll(PDO::FETCH_ASSOC);

			$query->closeCursor();

			/*-------------------------------------------------*/
			/* TYPE LIST					   */
			/*-------------------------------------------------*/

			foreach($lines as $line)
			{
				if(strncmp($line['CONSTRAINT_NAME'], 'fk_', 3) == 0)
				{
					array_push($result, array(
						'TABLE_NAME' => $line['TABLE_NAME'],
						'COLUMN_NAME' => $line['COLUMN_NAME'],
						'REFERENCED_TABLE_NAME' => $line['REFERENCED_TABLE_NAME'],
						'REFERENCED_COLUMN_NAME' => $line['REFERENCED_COLUMN_NAME'],
					));
				}
			}

			/*-------------------------------------------------*/
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

		return $result;
	}

	/*-----------------------------------------------------------------*/

	function my_print($level, $s)
	{
		while($level-- > 0)
		{
			print(' ');
		}

		print("$s");
	}

	/*-----------------------------------------------------------------*/

	function Sort($table)
	{
		return '';
	}

	/*-----------------------------------------------------------------*/

	function Coord($table)
	{
		return '';
	}

	/*-----------------------------------------------------------------*/

	function Patch($name, $value)
	{
		return $value;
	}

	/*-----------------------------------------------------------------*/

	function EmitHTMLContent($level)
	{
		/*---------------------------------------------------------*/
		/* SQL QUERIES						   */
		/*---------------------------------------------------------*/

		$nameList = $this->SQLFieldNameList();
		$typeList = $this->SQLFieldTypeList();

		$parentTableList = $this->SQLParentTableList($this->table);

		/*---------------------------------------------------------*/
		/* TITLE						   */
		/*---------------------------------------------------------*/

		$this->my_print($level, "<tr class=\"periph_builder_list_line_title\">\n");

		foreach($nameList as $name)
		{
			$this->my_print($level, "  <td>$name</td>\n");
		}

		$this->my_print($level, "</tr>\n");

		/*---------------------------------------------------------*/
		/* CONTENT						   */
		/*---------------------------------------------------------*/

		$i = 0;

		$lines = $this->SQLQuery("SELECT * FROM $this->table $this->filter1 $this->sort");

		foreach($lines as $line)
		{
			if($i++ % 2 != 0) {
				$this->my_print($level, "<tr class=\"periph_builder_list_line_odd\">\n");
			}
			else {
				$this->my_print($level, "<tr class=\"periph_builder_list_line_even\">\n");
			}

			$id = $line['id'];

			$nr = (count($nameList) + count($typeList)) / 2;

			for($j = 0; $j < $nr; $j++)
			{
				$isOk = false;

				$name = $nameList[$j];
				$type = $typeList[$j];

				$value = $line[$name];

				foreach($parentTableList as $parentTable)
				{
					if($parentTable['COLUMN_NAME'] == $name)
					{
						$tableName = $parentTable['REFERENCED_TABLE_NAME'];
						$columnName = $parentTable['REFERENCED_COLUMN_NAME'];

						if($columnName != 'id')
						{
							die("In table '$tableName', column 'id' expected but column '$columnName' found !\n");
						}

						$LINES = $this->SQLQuery("SELECT * FROM $tableName WHERE id = '$value'");

						foreach($LINES as $LINE)
						{
							$ID   = isset($LINE[ 'id' ]) ? $LINE[ 'id' ] : 0x1;
							$NAME = isset($LINE['name']) ? $LINE['name'] : $ID;

							$this->my_print($level, "  <td onclick=\"editLine('$this->db', '$this->table', $id);\">$NAME</td>\n");
						}

						$isOk = true;

						break;
					}
				}

				if($isOk == false)
				{
					if(strncmp($type, 'int', 3) == 0)
					{
						$value = $this->Patch($name, $value);
					}

					$this->my_print($level, "  <td onclick=\"editLine('$this->db', '$this->table', $id);\">$value</td>\n");
				}
			}

			$this->my_print($level, "</tr>\n");
		}

		/*---------------------------------------------------------*/
	}

	/*-----------------------------------------------------------------*/

	function EmitHTMLForm($level, $navigation = true)
	{
		$this->my_print($level, "<table style=\"width: 100%;\">\n");

		/*---------------------------------------------------------*/
		/* SQL QUERIES						   */
		/*---------------------------------------------------------*/

		$nameList = $this->SQLFieldNameList();
		$typeList = $this->SQLFieldTypeList();

		$parentTableList = $this->SQLParentTableList($this->table);
		$relatedTableList = $this->SQLRelatedTableList($this->table);

		/*---------------------------------------------------------*/
		/* CONTENT						   */
		/*---------------------------------------------------------*/

		$nr = (count($nameList) + count($typeList)) / 2;

		for($i = 0; $i < $nr; $i++)
		{
			$name = $nameList[$i];
			$type = $typeList[$i];

			$this->my_print($level, "  </tr>\n");

			$this->my_print($level, "    <td>$name:</td>\n");

			/*-------------------------------------------------*/
			/* FOREIGN KEY					   */
			/*-------------------------------------------------*/

			$isOk = false;

			foreach($parentTableList as $parentTable)
			{
				if($parentTable['COLUMN_NAME'] == $name)
				{
					$tableName = $parentTable['REFERENCED_TABLE_NAME'];
					$columnName = $parentTable['REFERENCED_COLUMN_NAME'];

					if($columnName != 'id')
					{
						die("In table '$tableName', column 'id' expected but column '$columnName' found !\n");
					}

					$this->my_print($level, "    <td>\n");

					$this->my_print($level, "      <select name=\"$name\" style=\"height: 22px; width: 100%;\">\n");

					$LINES = $this->SQLQuery($this->XXXX == $name ? "SELECT * FROM $tableName $this->filter2" : "SELECT * FROM $tableName");

					foreach($LINES as $LINE)
					{
						$ID   = isset($LINE[ 'id' ]) ? $LINE[ 'id' ] : 0x1;
						$NAME = isset($LINE['name']) ? $LINE['name'] : $ID;

						$this->my_print($level, "        <option value=\"$ID\">$NAME</option>\n");
					}

					$this->my_print($level, "      </select>\n");

					$this->my_print($level, "    </td>\n");

					$isOk = true;

					break;
				}
			}

			if($isOk == false)
			{
				/*-----------------------------------------*/
				/* INT & VARCHAR			   */
				/*-----------------------------------------*/

				/**/ if(strncmp($type, 'int', 3) == 0
					||
					strncmp($type, 'text', 4) == 0
					||
					strncmp($type, 'varchar', 7) == 0
				 ) {
					$this->my_print($level, "    <td><input type=\"text\" name=\"$name\" value=\"\" style=\"height: 22px; width: 99%;\"></td>\n");
				}

				/*-----------------------------------------*/
				/* ENUM					   */
				/*-----------------------------------------*/

				else if(strncmp($type, 'enum', 4) == 0)
				{
					$this->my_print($level, "    <td>\n");

					$this->my_print($level, "      <select name=\"$name\" style=\"height: 22px; width: 100%;\">\n");

					$items = explode(',', substr($type, +5, -1));

					foreach($items as $item)
					{
						$item = substr($item, +1, -1);

						$this->my_print($level, "        <option value=\"$item\">$item</option>\n");
					}

					$this->my_print($level, "      </select>\n");

					$this->my_print($level, "    </td>\n");
				}

				/*-----------------------------------------*/
				/* ELSE					   */
				/*-----------------------------------------*/

				else
				{
					$this->my_print($level, "    <td>$type</td>\n");
				}

				/*-----------------------------------------*/
			}

			/*-------------------------------------------------*/

			$this->my_print($level, "  </tr>\n");

			/*-------------------------------------------------*/
		}

		/*---------------------------------------------------------*/
		/* HIDDENS & BUTTONS					   */
		/*---------------------------------------------------------*/

		$this->my_print($level, "  <tr>\n");
		$this->my_print($level, "    <td style=\"text-align: left;\">\n");
		$this->my_print($level, "      <input type=\"hidden\" name=\"db\" value=\"$this->db\" />\n");
		$this->my_print($level, "      <input type=\"hidden\" name=\"login\" value=\"$this->login\" />\n");
		$this->my_print($level, "      <input type=\"hidden\" name=\"password\" value=\"$this->password\" />\n");
		$this->my_print($level, "      <input type=\"hidden\" name=\"table\" value=\"$this->table\" />\n");
		$this->my_print($level, "      <input type=\"hidden\" name=\"id\" value=\"\" />\n");
		$this->my_print($level, "      <input type=\"hidden\" name=\"parent_table\" value=\"$this->parent_table\" />\n");
		$this->my_print($level, "      <input type=\"hidden\" name=\"parent_id\" value=\"$this->parent_id\" />\n");
		$this->my_print($level, "    </td>\n");
		$this->my_print($level, "    <td style=\"text-align: right;\">\n");
		$this->my_print($level, "      <input type=\"reset\" value=\"Reset\" onclick=\"disableButtons();\" />\n");
		$this->my_print($level, "      &nbsp-&nbsp;\n");
		$this->my_print($level, "      <input type=\"submit\" name=\"remove\" value=\"Remove\" />\n");
		$this->my_print($level, "      <input type=\"submit\" name=\"update\" value=\"Update\" />\n");
		$this->my_print($level, "      <input type=\"submit\" name=\"append\" value=\"Append\" />\n");
		$this->my_print($level, "    </td>\n");
		$this->my_print($level, "  </tr>\n");

		/*---------------------------------------------------------*/

		$this->my_print($level, "</table>\n");

		if($navigation == false)
		{
			return;
		}

		$this->my_print($level, "<table style=\"width: 100%; border-top: solid 2px #EEEEEE;\">\n");

		/*---------------------------------------------------------*/
		/* LINKS						   */
		/*---------------------------------------------------------*/

		$this->my_print($level, "  <tr>\n");

		/*------------------------------*/
		/* PARENT LIST			*/
		/*------------------------------*/

		$this->my_print($level, "    <td style=\"text-align: left;\">\n");

		$nr = count($parentTableList);

		if($nr > 0)
		{
			$tmpList = array();

			$this->my_print($level, "      Parent tables:\n");

			for($i = 0; $i < $nr; $i++)
			{
				$TABLE = $parentTableList[$i]['REFERENCED_TABLE_NAME'];

				if(array_lookup_and_push($tmpList, $TABLE) == false)
				{
					if($i > 0)
					{
						$this->my_print($level, "      |\n");
					}

					$this->my_print($level, "      <a href=\"index.php?db=$this->db&login=$obj->login&password=$obj->password&table=$TABLE\" class=\"prev\">$TABLE</a>\n");
				}
			}
		}

		$this->my_print($level, "    </td>\n");

		/*------------------------------*/
		/* RELATED LIST			*/
		/*------------------------------*/

		$this->my_print($level, "    <td style=\"text-align: right;\">\n");

		$nr = count($relatedTableList);

		if($nr > 0)
		{
			$tmpList = array();

			$this->my_print($level, "      Related tables:\n");

			for($i = 0; $i < $nr; $i++)
			{
				$TABLE = $relatedTableList[$i]['TABLE_NAME'];

				if(array_lookup_and_push($tmpList, $TABLE) == false)
				{
					if($i > 0)
					{
						$this->my_print($level, "      |\n");
					}

					$this->my_print($level, "      <a href=\"index.php?db=$this->db&login=$obj->login&password=$obj->password&table=$TABLE\" class=\"next\">$TABLE</a>\n");
				}
			}
		}

		$this->my_print($level, "    </td>\n");

		/*------------------------------*/

		$this->my_print($level, "  </tr>\n");

		/*---------------------------------------------------------*/

		$this->my_print($level, "</table>\n");

		/*---------------------------------------------------------*/
		/* NAVIGATION						   */
		/*---------------------------------------------------------*/

		function A($obj, $level, $parentTableList, &$tmpList)
		{
			foreach($parentTableList as $parentTable)
			{
				$TABLE = $parentTable['REFERENCED_TABLE_NAME'];
				$COORD = $obj->Coord($TABLE);

				if(array_lookup_and_push($tmpList, $TABLE) == false)
				{
					$obj->my_print($level, "    <area shape=\"rect\" coords=\"$COORD\" href=\"index.php?db=$obj->db&login=$obj->login&password=$obj->password&table=$TABLE\" class=\"prev\" />\n");
				}

				A($obj, $level, $obj->SQLParentTableList($TABLE), $tmpList);
			}
		}

		/*---------------------------------------------------------*/

		function B($obj, $level, $relatedTableList, &$tmpList)
		{
			foreach($relatedTableList as $relatedTable)
			{
				$TABLE = $relatedTable['TABLE_NAME'];
				$COORD = $obj->Coord($TABLE);

				if(array_lookup_and_push($tmpList, $TABLE) == false)
				{
					$obj->my_print($level, "    <area shape=\"rect\" coords=\"$COORD\" href=\"index.php?db=$obj->db&login=$obj->login&password=$obj->password&table=$TABLE\" class=\"next\" />\n");
				}

/*				B($obj, $level, $obj->SQLRelatedTableList($TABLE), $tmpList);
 */			}
		}

		/*---------------------------------------------------------*/

		$tmpList = array();

		$this->my_print($level, "<p>\n");

		$this->my_print($level, "  <img src=\"img/sql-$this->table.png\" width=\"100%\" usemap=\"#nav_map\" />\n");

		$this->my_print($level, "  <map name=\"nav_map\">\n");

		A($this, $level, $parentTableList, $tmpList);

		B($this, $level, $relatedTableList, $tmpList);

		$this->my_print($level, "  </map>\n");

		$this->my_print($level, "</p>\n");


		/*---------------------------------------------------------*/
	}

	/*-----------------------------------------------------------------*/

	function Redirect()
	{
		if(strlen($this->parent_table) > 0
		   &&
		   strlen($this->parent_id) > 0
		 ) {
			header("Location: index.php?db=$this->db&table=$this->table&parent_table=$this->parent_table&parent_id=$this->parent_id");
		}
		else
		{
			header("Location: index.php?db=$this->db&table=$this->table");
		}

		exit(0);
	}

	/*-----------------------------------------------------------------*/

	function Remove($options, $redirect = true)
	{
		$id = $options['id'];

		$this->SQLQuery("DELETE FROM $this->table WHERE id='$id'");

		if($redirect != false)
		{
			$this->Redirect();
		}
	}

	/*-----------------------------------------------------------------*/

	function Append($options, $redirect = true)
	{
		$valueList = array();

		$nameList = $this->SQLFieldNameList();
		$typeList = $this->SQLFieldTypeList();

		$nr = (count($nameList) + count($typeList)) / 2;

		for($i = 0; $i < $nr; $i++)
		{
			$name = $nameList[$i];
			$type = $typeList[$i];

			$value = trim($options[$name]);

			/**/ if(strncmp($type, 'int', 3) == 0 && strlen($value) >= 2 && $value[0] == '0' && ($value[1] == 'x' || $value[1] == 'X'))
			{
				$value = hexdec($value);
			}
			else if(strncmp($type, 'int', 3) == 0 && strlen($value) >= 2 && $value[0] == '0' && ($value[1] == 'b' || $value[1] == 'B'))
			{
				$value = bindec($value);
			}

			array_push($valueList, "'$value'");
		}

		$list1 = implode(', ', $nameList);
		$list2 = implode(', ', $valueList);

		$this->SQLQuery("INSERT INTO $this->table ($list1) VALUES($list2)");

		if($redirect != false)
		{
			$this->Redirect();
		}
	}

	/*-----------------------------------------------------------------*/

	function Update($options, $redirect = true)
	{
		$dic = array();

		$nameList = $this->SQLFieldNameList($table);
		$typeList = $this->SQLFieldTypeList($table);

		$nr = (count($nameList) + count($typeList)) / 2;

		for($i = 0; $i < $nr; $i++)
		{
			$name = $nameList[$i];
			$type = $typeList[$i];

			$value = trim($options[$name]);

			/**/ if(strncmp($type, 'int', 3) == 0 && strlen($value) >= 2 && $value[0] == '0' && ($value[1] == 'x' || $value[1] == 'X'))
			{
				$value = hexdec($value);
			}
			else if(strncmp($type, 'int', 3) == 0 && strlen($value) >= 2 && $value[0] == '0' && ($value[1] == 'b' || $value[1] == 'B'))
			{
				$value = bindec($value);
			}

			array_push($dic, "$name='$value'");
		}

		$id = $options['id'];

		$list = implode(', ', $dic);

		$this->SQLQuery("UPDATE $this->table SET $list WHERE id='$id'");

		if($redirect != false)
		{
			$this->Redirect();
		}
	}

	/*-----------------------------------------------------------------*/

	function EmitJSONContent($id)
	{
		try
		{
			/*-------------------------------------------------*/
			/* SQL QUERY					   */
			/*-------------------------------------------------*/

			$query = $this->db_table->query("SELECT * FROM $this->table WHERE id='$id'");

			$data = $query->fetch(PDO::FETCH_ASSOC);

			$query->closeCursor();

			/*-------------------------------------------------*/
			/* PATCH					   */
			/*-------------------------------------------------*/

			foreach($data as $name => &$value)
			{
				$value = $this->Patch($name, $value);
			}

			/*-------------------------------------------------*/
			/* JSON						   */
			/*-------------------------------------------------*/

			header('Content-type: application/json');

			echo json_encode($data);

			/*-------------------------------------------------*/
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

		exit(0);
	}

	/*-----------------------------------------------------------------*/
}

/*-------------------------------------------------------------------------*/
?>