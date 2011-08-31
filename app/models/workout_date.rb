class WorkoutDate < ActiveRecord::Base
  include Csv

  belongs_to :workout
  has_many :exercise_log_entries
  has_many :set_goals

  after_create :set_default_goals

  def self.write_csv
    super do
      WorkoutDate.where("finished_at IS NULL").order('`when`').uniq
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

  def last
    return @last_wd if @last_wd
    # find one earlier workout date
    @last_wd = workout.workout_dates.find_by_n(self.n - 1)
  end

  def goal(set, variable, params = {})
    set_goal = set_goals.find_by_set_id_and_variable(set.try(:id), variable)
    if params[:set_default]
      # find one earlier workout date
      workout = set_goal.workout_date.workout
      last_wd = workout.workout_dates.find_by_n(self.n - 1)
      # find one earlier workout set goal
      last_goal = last_wd.set_goals.find_by_set_id_and_variable(set.try(:id), variable)
      # copy value
      workout_date.variable = last_wd.variable
      workout_date.save!
    end
    set_goal.try(:value)
  end

  def set_default_goals

    if last_wd
      self.workout.exercises.each do |ex|
        @workout_date.exercise_log_entries.find_or_create_by_exercise_id(exercise)

        ex.sets.each do |set|
          sle = set.set_log_entries.find_or_create_by_exercise_log_entry_id(ele.id)
        end
      end
    end
  end
end
