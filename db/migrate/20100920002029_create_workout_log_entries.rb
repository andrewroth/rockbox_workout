class CreateWorkoutLogEntries < ActiveRecord::Migration
  def self.up
    create_table :workout_log_entries do |t|
      t.integer :workout_id
      t.integer :n

      t.timestamps
    end
  end

  def self.down
    drop_table :workout_log_entries
  end
end
