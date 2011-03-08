class WorkoutDate < ActiveRecord::Base
  include Csv

  belongs_to :workout
  has_many :exercise_log_entries

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL", :order => :when).uniq
    end
  end

  def self.read_csv
    f = File.read(Rails.root.join("transfer/workout_dates.csv"))
    wd = nil
    f.split("\n").each do |line|
      name, type, value = line.split(",")
      case name
      when 'id'
        # save the previous workout date
        if wd
          wd.save!
        end

        wd = WorkoutDate.find_by_id value
        #wd = WorkoutDate.find_or_create_by_id value
      when 'started_at_int'
        wd.started_at = Time.at(value.to_i) if wd && value != '0'
      when 'created_at_int'
        wd.created_at = Time.at(value.to_i) if wd && value != '0'
      when 'finished_at_int'
        wd.finished_at = Time.at(value.to_i) if wd && value != '0'
      end
    end
    wd.save! if wd # save the last workout date
  end

  def self.maximum_n_from_workout_id(workout_id)
    self.maximum(:n, :conditions => [ 'workout_id = ?', workout_id ])
  end

  def self.next_free_n(workout_id)
    max_n = self.maximum_n_from_workout_id(workout_id)
    max_n ? max_n + 1 : 0
  end
end
