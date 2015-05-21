<?php
/**
 * @file
 * Migrate class to create Organisation entities.
 */

/**
 * Class to create the organisation entities from CSV
 */
class C4dMigrateCreateCSVTermTopic extends C4dMigrateCreateBaseCSVVocabularyTerms {
  /**
   * CSV column mapping.
   *
   * @var array
   */
  public $csvColumns = array(
    array('id', 'ID'),
    array('parent', 'Parent'),
    array('weight', 'Weight'),
    array('title', 'Title'),
    array('description', 'Description'),
  );

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Create topic terms.');
    $this->setTargetBundle('c4m_vocab_topic');

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Use the migration mapping to link child with parent.
    $this->addFieldMapping('parent', 'parent')
      ->sourceMigration($this->machineName);
  }
}