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

    $this->addFieldMapping(
      'c4m_location_address',
      'c4m_location_address_country'
    );
    $this->addFieldMapping(
      'c4m_location_address:thoroughfare',
      'c4m_location_address_street'
    );
    $this->addFieldMapping(
      'c4m_location_address:premise',
      'c4m_location_address_additional'
    );
    $this->addFieldMapping(
      'c4m_location_address:postal_code',
      'c4m_location_address_postal'
    );
    $this->addFieldMapping(
      'c4m_location_address:locality',
      'c4m_location_address_city'
    );
    $this->addFieldMapping(
      'c4m_location_address:administrative_area',
      'c4m_location_address_state'
    );
    $this->addFieldMapping(
      'c4m_location_address:name_line',
      'c4m_location_address_name'
    );
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'field_event_full_description')
      ->arguments(array('format' => 'filtered_html'));
    $this->addFieldMapping('c4m_body:summary', 'body')
      ->arguments(array('format' => 'filtered_html'));

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

    $row->field_ref_document = unserialize($row->field_ref_document);
    $row->field_event_location = unserialize($row->field_event_location);
    $row->field_event_organiser = unserialize($row->field_event_organiser);

    $row->body = $this->prepareHtml($row->body);
    $row->field_event_full_description = $this->prepareHtml(
      $row->field_event_full_description
    );

    if (!$row->field_event_location == '' && !empty($row->field_event_location)) {

      $location = $row->field_event_location;

      $mapping = array(
        'UK' => 'GB',
      );

      $country = strtoupper($location['country']);

      $row->c4m_location_address_country = (!empty($mapping[$country])) ? $mapping[$country] : $country;
      $row->c4m_location_address_street = $location['street'];
      $row->c4m_location_address_additional = $location['additional'];
      $row->c4m_location_address_postal = $location['postal_code'];
      $row->c4m_location_address_city = $location['city'];
      $row->c4m_location_address_state = $location['province'];
      $row->c4m_location_address_name = $location['name'];
    }

    return TRUE;
  }

  /**
   * Calls parent complete function.
   */
  public function complete($entity, $row) {
    return parent::complete($entity, $row);
  }

}
