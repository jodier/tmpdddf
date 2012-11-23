<?php

namespace Hypnos\SupportBundle\Controller;

use Symfony\Bundle\FrameworkBundle\Controller\Controller;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\HttpFoundation\Request;

class DefaultController extends Controller
{
	/*-----------------------------------------------------------------*/

	public function getPathItemArray()
	{
		$request = Request::createFromGlobals();

		$pathItemArray = preg_split('/\//', $request->getPathInfo());

		/**/

		$result = array();

		foreach($pathItemArray as $pathItem)
		{
			if(strcmp($pathItem, 'home') != 0 && strlen($pathItem) > 0)
			{
				array_push($result, $pathItem);
			}
		}

		return $result;
	}

	/*-----------------------------------------------------------------*/
	/* INDEX							   */
	/*-----------------------------------------------------------------*/

	public function indexAction($page)
	{
		$pathItemArray = $this->getPathItemArray();

		return $this->container->get('templating')->renderResponse('HypnosSupportBundle:Default:index.htm.twig', array(
			'pathItemArray' => $pathItemArray,
			'page' => $page,
		));
	}

	/*-----------------------------------------------------------------*/
}
