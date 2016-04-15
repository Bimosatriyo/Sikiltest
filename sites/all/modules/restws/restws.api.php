<?php

/**
 * @file
 */


/**
 * @defgroup restws RestWS module integrations.
 */

/**
 * @defgroup restws_hooks RestWS' hooks
 */

/**
 * @return array
 * @see MyModuleBookResourceController
 */
function hook_restws_resource_info() {
  return array(
    'mymodule_book' => array(
      'label' => t('Book'),
      'class' => 'MyModuleBookResourceController',
      'menu_path' => 'api/mybook',
    ),
    'mymodule_status' => array(
      'label' => t('Status'),
      'class' => 'MyModuleStatusResourceController',
    ),
  );
}

/**
 * @param array $resource_info
 * @see hook_restws_resource_info()
 */
function hook_restws_resource_info_alter(&$resource_info) {
  $resource_info['node']['class'] = 'MySpecialNodeResourceController';
  $resource_info['node']['menu_path'] = 'mypath';
}

/**
 * @return array
 */
function hook_restws_format_info() {
  return array(
    'json' => array(
      'label' => t('JSON'),
      'class' => 'RestWSFormatJSON',
      'mime type' => 'application/json',
    ),
    'xml' => array(
      'label' => t('XML'),
      'class' => 'RestWSFormatXML',
      'mime type' => 'application/xml',
    ),
  );
}

/**
 * @param array $format_info
 * @see hook_restws_format_info()
 */
function hook_restws_format_info_alter(&$format_info) {
  $format_info['json']['class'] = 'MyJsonFormatHandler';
}

/**
 * @param array $request
 */
function hook_restws_request_alter(array &$request) {
  if ($request['resource']->resource() == 'node') {
    $request['format'] = restws_format('json');
  }
}

/**
 * @param mixed $response
 * @param string $function
 * @param string $format
 * @param RestWSResourceControllerInterface $resourceController
 */
function hook_restws_response_alter(&$response, $function, $formatName, $resourceController) {
  if ($function == 'viewResource' && $formatName == 'json') {
    $response['site_name'] = variable_get('site_name', '');
  }
}

/**
 * @param array $controls
 */
function hook_restws_meta_controls_alter(&$controls) {
  $controls['deep-load-refs'] = 'deep-load-refs';
}

/**
 * @}
 */

/**
 * ontroller class mymodule_book.
 */
class MyModuleBookResourceController implements RestWSResourceControllerInterface {

  /**
   * @see hook_entity_property_info()
   * @see RestWSResourceControllerInterface::propertyInfo()
   */
  public function propertyInfo() {
    return array(
      'properties' => array(
        'title' => array(
          'type' => 'text',
          'label' => t('Book title'),
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'author' => array(
          'type' => 'text',
          'label' => t('Author'),
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'pages' => array(
          'type' => 'integer',
          'label' => t('Number of pages'),
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'price' => array(
          'type' => 'decimal',
          'label' => t('Price'),
          'setter callback' => 'entity_property_verbatim_set',
        ),
      ),
    );
  }

  /**
   * @see RestWSResourceControllerInterface::wrapper()
   */
  public function wrapper($id) {
    $book = mymodule_book_load($id);
    $info = $this->propertyInfo();
    return entity_metadata_wrapper('mymodule_book', $book, array('property info' => $info['properties']));
  }

  /**
   * @see RestWSResourceControllerInterface::create()
   */
  public function create(array $values) {
    try {
      $book = mymodule_book_save($values);
      return $book->id;
    }
    catch (Exception $e) {
      throw new RestWSException('Creation error', 406);
    }
  }

  /**
   * @see RestWSResourceControllerInterface::read()
   */
  public function read($id) {
    return mymodule_book_load($id);
  }

  /**
   * @see RestWSResourceControllerInterface::update()
   */
  public function update($id, array $values) {
    throw new RestWSException('Not implemented', 501);
  }

  /**
   * @see RestWSResourceControllerInterface::delete()
   */
  public function delete($id) {
    try {
      mymodule_book_delete($id);
    }
    catch (Exception $e) {
      throw new RestWSException('Book not found', 404);
    }
  }

  /**
   * @see RestWSResourceControllerInterface::access()
   */
  public function access($op, $id) {
    return mymodule_book_access($op, $id);
  }

  /**
   * @see RestWSResourceControllerInterface::resource()
   */
  public function resource() {
    return 'mymodule_book';
  }
}
