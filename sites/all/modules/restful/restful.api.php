<?php

/**
 * @file
 */

/**
 * @addtogroup hooks
 */


/**
 * Allow altering the request before it is processed.
 */
function hook_restful_parse_request_alter(\Drupal\restful\Http\RequestInterface &$request) {
  $request->setApplicationData('csrf_token', 'token');
}

/**
 * Allow altering the request before it is processed.
 */
function hook_restful_resource_alter(\Drupal\restful\Plugin\resource\ResourceInterface &$resource) {
  $plugin_definition = $resource->getPluginDefinition();
  if (!empty($plugin_definition['renderCache']) && !empty($plugin_definition['renderCache']['render'])) {
    $resource = new \Drupal\restful\Plugin\resource\CachedResource($resource);
  }
}

/**
 * @} End of "addtogroup hooks".
 */
