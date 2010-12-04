class MergeWorkoutLogEntriesToDates < ActiveRecord::Migration
  def self.up
    drop_table :workout_log_entries
    add_column :workout_dates, :n, :integer
  end

  def self.down
    create_table :workout_log_entries do |t|
      t.integer :workout_id
      t.integer :n
      t.timestamps
    end
    remove_column :workout_dates, :n, :integer
  end
end
