<?php

/**
 * @file
 * The UI controller for the inventory item entity.
 */

class DrpgInventoryItemUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = t('Manage Inventory Items.');
    return $items;
  }
}