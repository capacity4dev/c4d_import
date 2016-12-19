<?php
/**
 * @file
 * Migrate class to import OG features.
 */

/**
 * Class C4dMigrateImportOGFeatures.
 */
class C4dMigrateImportOGFeatures extends C4dMigrateImportBase {

  /**
   * Content creations need to be run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportUsers',
    'C4dMigrateImportNodeGroup',
    'C4dMigrateImportNodeProject',
  );

  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import OG features from our Drupal 6 database.');

    $this->sourceTable = 'spaces_overrides';
    $this->targetTable = 'variable_store';

    $query = $this->createSelect($this->sourceTable, 'o')
                  ->fields('o', array('id', 'object_id', 'value'))
                  ->condition('type', 'og')
                  ->condition('object_id', 'spaces_features')
                  ->orderBy('id', 'ASC');

    $key = array(
      'id' => array(
        'description' => t('The space id.'),
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
      'object_id' => array(
        'description' => t('The override object id.'),
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
    $this->map         = new MigrateSQLMap($this->machineName, $key, MigrateDestinationTable::getKeySchema($this->targetTable));
    $this->source      = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTable($this->targetTable);

    $this->addFieldMapping('realm', '_realm');
    $this->addFieldMapping('realm_key', '_realm_key');
    $this->addFieldMapping('name', '_name');
    $this->addFieldMapping('value', '_value');
    $this->addFieldMapping('serialized', '_serialized');
  }

  /**
   * Override Migration::preImport().
   *
   * @todo survey, emeeting, projects, photoalbum.
   */
  protected function preImport() {
    parent::preImport();

    // Set up mapping between old & new features.
    $this->featureMappings = array(
      'ec_blog' => 'c4m_features_og_discussions',
      'ec_calendar' => 'c4m_features_og_events',
      'ec_dashboard' => NULL,
      'ec_documents' => 'c4m_features_og_documents',
      'ec_members' => 'c4m_features_og_members',
      'ec_minisite' => 'c4m_features_og_wiki',
      'ec_survey' => NULL,
      'ec_emeeting' => NULL,
      'ec_projects' => NULL,
      'ec_photoalbum' => 'c4m_features_og_media',
    );

    // Set up default features.
    $this->defaultFeatures = array(
      'c4m_features_og_wiki' => 'c4m_features_og_wiki',
      'c4m_features_og_discussions' => 'c4m_features_og_discussions',
      'c4m_features_og_documents' => 'c4m_features_og_documents',
      'c4m_features_og_events' => 'c4m_features_og_events',
      'c4m_features_og_members' => 'c4m_features_og_members',
      'c4m_features_og_media' => 'c4m_features_og_media',
      'c4m_features_og_news' => 'c4m_features_og_news',
    );
  }

  /**
   * {@inheritdoc}
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    // Get the group id.
    $q    = $this->createSelect('node', 'n');
    $type = $q->fields('n', array('type'))->condition('nid', $row->id, '=')
      ->execute()->fetchField();
    switch ($type) {
      case 'project_group':
        $handler = 'C4dMigrateImportNodeProject';
        break;

      case 'group':
        $handler = 'C4dMigrateImportNodeGroup';
        break;
    }
    $gid = $this->handleSourceMigration($handler, $row->id, NULL, $this);
    if (empty($gid)) {
      watchdog(C4D_MIGRATE_MIGRATION_GROUP, "Group doesn't exist: " . $row->id);
      return FALSE;
    }

    // Check if features were overridden.
    $sourceFeatures = unserialize($row->value);
    if (empty($sourceFeatures)) {
      return FALSE;
    }

    // Initialize the default settings.
    $features = $this->defaultFeatures;

    // Convert feature settings.
    foreach ($sourceFeatures as $feature => $enabled) {
      $key = $features[$this->featureMappings[$feature]];
      if (empty($key)) {
        // @todo: Group feature not yet available.
        watchdog(C4D_MIGRATE_MIGRATION_GROUP, 'Group feature not yet available. ' . $feature);
        continue;
      }
      if ($enabled) {
        $features[$key] = $features[$key];
      }
      else {
        $features[$key] = 0;
      }
    }

    // Prepare row data.
    $row->_realm = 'og';
    $row->_realm_key = 'node_' . $gid;
    $row->_name = 'c4m_og_features_group';
    $row->_value = serialize($features);
    $row->_serialized = 1;

    return TRUE;
  }

}
