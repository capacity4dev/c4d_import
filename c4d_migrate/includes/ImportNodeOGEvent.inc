<?php
/**
 * @file
 * Migrate class to import the Event group nodes.
 */

/**
 * Class to import the Event group nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeOGEvent extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param object $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Event group nodes.');
    $this->setSourceTable('c4d_export_node_event');
    $this->setTargetBundle('event');

    $this->dependencies[] = 'C4dMigrateImportNodeOGDocument';

    $this->setCategoriesAndTags(TRUE);

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'field_event_full_description')
         ->arguments(array('format' => 'full_html'));
    $this->addFieldMapping('c4m_body:summary', 'body')
         ->arguments(array('format' => 'full_html'));

    $this->addFieldMapping('c4m_organised_by', 'field_event_organiser');

    $this->addFieldMapping('c4m_datetime_end', 'field_date_start');
    $this->addFieldMapping('c4m_datetime_end:to', 'field_date_end');

    $this->addFieldMapping('c4m_related_document', 'field_ref_document')
         ->sourceMigration('C4dMigrateImportNodeOGDocument');

    $this->addFieldMapping('c4m_event_type')
      ->defaultValue('event');
  }

  /**
   * Prepare the row data loaded from the table.
   *
   * @param object $row
   *   The data from the table.
   *
   * @return bool
   *   Success.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    $row->field_ref_document    = unserialize($row->field_ref_document);
    $row->field_event_location  = unserialize($row->field_event_location);
    $row->field_event_organiser = unserialize($row->field_event_organiser);

    $row->body                         = $this->prepareHtml($row->body);
    $row->field_event_full_description = $this->prepareHtml($row->field_event_full_description);

    return TRUE;
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $row
   *   The data row we are importing.
   *
   * @return bool
   *   Successfully completed or not.
   */
  public function complete($entity, $row) {
    if ($row->field_event_location == '' || empty($row->field_event_location)) {
      parent::complete($entity, $row);
      return FALSE;
    }

    $location = $row->field_event_location;

    $criteria = array(
      'field_name' => 'c4m_location',
      'vid'        => $entity->vid,
      'nid'        => $entity->nid,
    );

    $field    = field_info_field('c4m_location');
    $instance = field_info_instance('node', 'c4m_location', $entity->type);

    $settings = FALSE;
    if (isset($field['settings'])) {
      $settings = $field['settings'];
    }

    $settings['display_settings'] = getlocations_fields_get_display_settings($instance['display']['default']['settings']);

    include_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'location') . '/geocoding/google.inc';
    $parsed = google_geocode_location($location);

    $locations = array(
      array(
        'name'        => $location['name'],
        'street'      => $location['street'],
        'additional'  => $location['additional'],
        'city'        => $location['city'],
        'province'    => $location['province'],
        'postal_code' => $location['postal_code'],
        'country'     => strtoupper($location['country']),
      ),
    );

    $mapping = array(
      'UK' => 'GB',
      'uk' => 'gb',
    );

    if (!empty($location['latitude']) && $location['latitude'] != 0.000000) {
      $locations[0]['latitude'] = $location['latitude'];
    }
    else {
      $locations[0]['latitude'] = $parsed['lat'];
    }
    if (!empty($location['longitude']) && $location['longitude'] != 0.000000) {
      $locations[0]['longitude'] = $location['longitude'];
    }
    else {
      $locations[0]['longitude'] = $parsed['lon'];
    }

    if (!empty($mapping[$locations[0]['country']])) {
      $locations[0]['country'] = $mapping[$locations[0]['country']];
    }

    $items = getlocations_fields_save_locations($locations, $criteria, $settings, 'insert');

    if (is_array($items) && !empty($items[0])) {
      $entity->c4m_location                   = array(LANGUAGE_NONE => array());
      $entity->c4m_location[LANGUAGE_NONE][0] = $items[0];
      $entity->c4m_location[LANGUAGE_NONE][0]['active'] = TRUE;

      $this->nodeSave($entity, $row->changed);
    }

    parent::complete($entity, $row);

    return TRUE;
  }

}
