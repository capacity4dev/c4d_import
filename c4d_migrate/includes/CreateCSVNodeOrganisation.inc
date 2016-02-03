<?php
/**
 * @file
 * Migrate class to create Organisation entities.
 */

/**
 * Class to create the organisation entities from CSV.
 */
class C4dMigrateCreateCSVNodeOrganisations extends C4dMigrateCreateBaseCSVNode {
  /**
   * CSV column mapping.
   *
   * @var array
   */
  public $csvColumns
    = array(
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
    $this->addFieldMapping('c4m_icon', 'image');
    $this->addFieldMapping('c4m_icon:file_replace')
      ->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('c4m_icon:source_dir')
      ->defaultValue(
        drupal_get_path('module', 'c4d_migrate') . '/includes/images'
      );
  }

  /**
   * The entity has been created but not yet saved.
   *
   * @param object $entity
   *   The entity object that was created.
   * @param object $row
   *   The CSV row we are manipulating.
   */
  public function prepare($entity, $row) {
    $this->extractDomain($entity, $row, 'domain');
  }

  /**
   * Helper method to combine a domain icon state and domain values for a "domain" field.
   *
   * @param object $entity
   *   The entity object AFTER it has been saved (nid available).
   * @param object $row
   *   The source row we are processing.
   * @param string $domainfield
   *   Name of the field holding the domain data.
   */
  public function extractDomain($entity, $row, $domainfield) {
    $domains = explode(',', $row->$domainfield);
    if (count($domains) < 1) {
      return FALSE;
    }

    foreach ($domains as $domain) {
      $domain_data = explode('|', $domain);
      if (count($domain_data) > 1) {
        $entity->c4m_domain[LANGUAGE_NONE][] = array(
          'icon'    => $domain_data[0],
          'domain'  => $domain_data[1],
        );
      }
      else {
        $entity->c4m_domain[LANGUAGE_NONE][] = array(
          'icon'    => 0,
          'domain'  => $domain_data[0],
        );
      }
    }
  }

}
