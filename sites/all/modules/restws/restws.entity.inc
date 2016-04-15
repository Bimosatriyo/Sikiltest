<?php

/**
 * @file
 */

/**
 * Specifies CRUD and access methods for resources.
 */
interface RestWSResourceControllerInterface {

  /**
   * @return array
   */
  public function propertyInfo();

  /**
   * @return EntityStructureWrapper
   */
  public function wrapper($id);

  /**
   * @param array $values
   * @return int|string
   */
  public function create(array $values);

  /**
   * @param int|string $id
   * @return
   */
  public function read($id);

  /**
   * @param int|string $id
   * @param array $values
   */
  public function update($id, array $values);

  /**
   * @param int|string $id
   */
  public function delete($id);

  /**
   * @param string $op
   * @param int|string $id
   * @see entity_access()
   */
  public function access($op, $id);

  /**
   * Returns the name of the resource.
   */
  public function resource();
}

/**
 * Specifies query methods for resources.
 */
interface RestWSQueryResourceControllerInterface extends RestWSResourceControllerInterface {
  /**
   * @param array $filters
   * @param array $meta_controls
   *   @see restws_meta_controls()
   * @return array
   */
  public function query($filters = array(), $meta_controls = array());

  /**
   * @param array $filters
   * @return int
   */
  public function count($filters = array());

  /**
   * @param int $client_limit
   * @return int
   */
  public function limit($client_limit = NULL);
}

/**
 * Controller for entity-bases resources.
 */
class RestWSEntityResourceController implements RestWSQueryResourceControllerInterface {

  protected $entityType, $entityInfo;

  public function __construct($name, $info) {
    $this->entityType = $name;
    $this->entityInfo = entity_get_info($name);
  }

  public function propertyInfo() {
    return entity_get_all_property_info($this->entityType);
  }

  public function wrapper($id) {
    return entity_metadata_wrapper($this->entityType, $id);
  }

  public function read($id) {
    return $this->wrapper($id)->value();
  }

  public function create(array $values) {
    $entity_info = entity_get_info($this->entityType);
    if (isset($entity_info['bundle keys'])) {
      foreach ($entity_info['bundle keys'] as $bundle_key) {
        if (!array_key_exists($bundle_key, $values)) {
          throw new RestWSException('Missing bundle: ' . $bundle_key, 406);
        }
      }
    }

    try {
      $wrapper = entity_property_values_create_entity($this->entityType, $values);
      $entity_keys = array_intersect_key($entity_info['entity keys'], array('id' => 1, 'bundle' => 1));

      foreach (array_keys($values) as $name) {
        if (!in_array($name, $entity_keys)) {
          if (!$this->checkPropertyAccess($wrapper, $name, $wrapper->{$name})) {
            throw new RestWSException(t('Not authorized to set property @p', array('@p' => $name)), 403);
          }
        }
      }
    }
    catch (EntityMetadataWrapperException $e) {
      throw new RestWSException($e->getMessage(), 406);
    }

    $properties = $wrapper->getPropertyInfo();
    $diff = array_diff_key($values, $properties);
    if (!empty($diff)) {
      throw new RestWSException('Unknown data properties: ' . implode(' ', array_keys($diff)) . '.', 406);
    }
    $wrapper->save();
    return $wrapper->getIdentifier();
  }

  public function update($id, array $values) {
    $wrapper = $this->wrapper($id);
    $entity_info = $wrapper->entityInfo();
    $entity_keys = array_intersect_key($entity_info['entity keys'], array('id' => 1, 'bundle' => 1));
    try {
      foreach ($values as $name => $value) {
        if (in_array($name, $entity_keys)) {
          // We don't allow changing the entity ID or bundle.
          if ($wrapper->{$name}->value() != $value) {
            throw new RestWSException('Unable to change ' . $name, 422);
          }
        }
        else {
          $wrapper->{$name}->set($value);
          if (!$this->checkPropertyAccess($wrapper, $name, $wrapper->{$name})) {
            throw new RestWSException(t('Not authorized to set property @p', array('@p' => $name)), 403);
          }
        }
      }
    }
    catch (EntityMetadataWrapperException $e) {
      throw new RestWSException($e->getMessage(), 406);
    }
    $wrapper->save();
  }

  public function delete($id) {
    entity_delete($this->entityType, $id);
  }

  /**
   * Implements RestWSQueryResourceControllerInterface::query().
   */
  public function query($filters = array(), $meta_controls = array()) {
    $limit = variable_get('restws_query_max_limit', 100);
    $offset = 0;

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $this->entityType);

    foreach ($filters as $filter => $value) {
      $this->propertyQueryOperation($query, 'Condition', $filter, $value);
    }

    $rest_controls = restws_meta_controls();
    foreach ($meta_controls as $control_name => $value) {
      switch ($control_name) {
        case $rest_controls['sort']:
          if (isset($meta_controls[$rest_controls['direction']]) && strtolower($meta_controls[$rest_controls['direction']]) == 'desc') {
            $direction = 'DESC';
          }
          else {
            $direction = 'ASC';
          }
          $this->propertyQueryOperation($query, 'OrderBy', $value, $direction);
          break;

        case $rest_controls['limit']:
          $limit = $this->limit($value);
          break;

        case $rest_controls['page']:
          $offset = $value > 0 ? $value : $offset;
          break;
      }
    }

    $offset *= $limit;
    $query->range($offset, $limit);

    $this->nodeAccess($query);

    try {
      $query_result = $query->execute();
    }
    catch (PDOException $exception) {
      throw new RestWSException('Query failed.', 400);
    }
    $query_result = isset($query_result[$this->entityType]) ? $query_result[$this->entityType] : array();

    $result = array_keys($query_result);

    return $result;
  }


  /**
   * Implements RestWSQueryResourceControllerInterface::count().
   */
  public function count($filters = array()) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $this->entityType);

    foreach ($filters as $filter => $value) {
      $this->propertyQueryOperation($query, 'Condition', $filter, $value);
    }
    $query->count();
    $this->nodeAccess($query);

    return $query->execute();
  }

  /**
   * @param EntityFieldQuery $query
   */
  protected function nodeAccess(EntityFieldQuery $query) {
    if ($this->resource() == 'node') {
      $query->addTag('node_access');
      if (!user_access('bypass node access')) {
        $this->propertyQueryOperation($query, 'Condition', 'status', 1);
      }
    }
  }

  /**
   * Implements RestWSQueryResourceControllerInterface::limit().
   */
  public function limit($client_limit = NULL) {
    $limit = variable_get('restws_query_max_limit', 100);
    if (!empty($client_limit) && $client_limit < $limit) {
      return $client_limit;
    }
    else {
      return $limit;
    }
  }

  public function access($op, $id) {
    return entity_access($op, $this->entityType, isset($id) ? $this->wrapper($id)->value() : NULL);
  }

  public function resource() {
    return $this->entityType;
  }

  /**
   * @param EntityFieldQuery $query
   * @param string $operation
   * @param string $property
   * @param string|array $value
   */
  protected function propertyQueryOperation(EntityFieldQuery $query, $operation, $property, $value) {
    $properties = $this->propertyInfo();

    if (empty($properties[$property]['field'])) {
      $column = $properties[$property]['schema field'];
      $operation = 'property' . $operation;
      $query->$operation($column, $value);
    }
    else {
      $field_info = field_info_field($property);
      $operation = 'field' . $operation;
      if (is_array($value)) {
        foreach ($value as $column => $val) {
          $query->$operation($field_info, $column, $val);
        }
      }
      else {
        $columns = array_keys($field_info['columns']);
        $column = $columns[0];
        $query->$operation($field_info, $column, $value);
      }
    }
  }

  /**
   * @todo Remove this once Entity API properly handles text format access.
   * @param EntityMetadataWrapper $entity
   * @param string $property_name
   * @param EntityMetadataWrapper $property
   * @return bool
   */
  protected function checkPropertyAccess($entity, $property_name, $property) {
    global $user;
    if ($entity->type() == 'node' && $property_name == 'author' && $property->raw() == $GLOBALS['user']->uid) {
      return TRUE;
    }
    elseif ($property->type() == 'text_formatted' && $property->format->value()) {
      $format = (object) array('format' => $property->format->value());
      if (!filter_access($format)) {
        return FALSE;
      }
    }
    $info = $property->info();
    if (!empty($info['access callback'])) {
      global $user;
      $data = $entity->value();
      return call_user_func($info['access callback'], 'edit', $property_name, $data, $user, $entity->type());
    }
    elseif (isset($info['setter permission'])) {
      return user_access($info['setter permission']);
    }
    return TRUE;
  }
}
