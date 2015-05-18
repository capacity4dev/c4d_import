<?php
/**
 * @file
 * Migrate class to create Organisation entities.
 */

/**
 * Class to create the organisation entities from CSV
 */
class C4dMigrateCreateCSVNodeOrganisations extends C4dMigrateCreateBaseCSVNode {
  /**
   * CSV column mapping.
   *
   * @var array
   */
  public $csvColumns = array(
    array('id', 'ID'),
    array('title', 'Title'),
    array('body', 'Body'),
    array('image', 'Icon'),
    array('domain', 'Domain'),
    array('uid', 'UID'),
  );

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Create organisation nodes.');
    $this->setTargetBundle('organisation');

    parent::__construct($group);

    $this->setPathManual(FALSE);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    $this->addFieldMapping('c4m_body', 'body')
      ->arguments(array('format' => 'full_html'));
    $this->addFieldMapping('c4m_link', 'link');
    $this->addFieldMapping('c4m_domain', 'domain')
      ->separator(',');
    $this->addFieldMapping('c4m_icon', 'image');
    $this->addFieldMapping('c4m_icon:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('c4m_icon:source_dir')
      ->defaultValue(drupal_get_path('module', 'c4d_migrate') . '/includes/images');
  }
}