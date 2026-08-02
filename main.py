import sys

from app.scraper import search_salary_job, search_salary_with_ai
from app.taxCalculator import calculate_tax
from app.ui import display_welcome_box, select_research_mode, searching_salary_ai, ai_error_message, invalid_characters_error, select_seniority, gross_annual_salary, monthly_calculated_neat_salary, agree_to_continue, loading_bar, spending_profile_header, ask_expense, spending_summary, ask_text, objectives_planning_header, ask_objectives_count, objective_header, ask_objective_name, ask_objective_cost, ask_objective_percentage, allocation_report, allocation_note, clear_screen
from app.ui import continue_program, logging_out, mothly_expenses_exceeded, savings, negative_input, numerical_value, searching_salary_jobdatalake, income_tax_for_country, invalid_input
SENIORITY_OPTIONS = ["Senior", "Mid level", "Manager", "Entry", "Staff"]
UNSAFE_CHARS = "\"`$;|<>&@'\\"


def contains_unsafe_characters(value: str) -> bool:
    return any(c in UNSAFE_CHARS for c in value)

def plan_objectives(savings_capacity: float) -> None:
    objectives_planning_header()

    print(" ")
    num = ask_objectives_count()

    names = []
    costs = []
    percentages = []
    total_percentage = 0.0

    for i in range(num):
        objective_header(i + 1)

        name = ask_objective_name()

        cost = ask_objective_cost()

        pct = ask_objective_percentage()

        names.append(name)
        costs.append(cost)
        percentages.append(pct)
        total_percentage += pct

    if total_percentage > 100.0:
        print(f"\n[WARNING]: The total allocation ({total_percentage:.1f}%) exceeds 100% of your savings!")
        print("Please restart the planning and balance your percentages properly.\n")
        return

    if total_percentage < 100.0:
        allocation_note(total_percentage)

    allocation_report(names, costs, percentages, savings_capacity)


def main():
    clear_screen()
    display_welcome_box()
    print()
    if "--help" in sys.argv or "-h" in sys.argv:
        print("Usage: python main.py [\033[92m--refresh\033[0m]")
        print()
        print("  \033[92m--refresh\033[0m   Re-extract the tax brackets for the selected country,")
        print("              overriding the saved cache entry (PDF or web search).")
        sys.exit(0)

    refresh = "--refresh" in sys.argv
    if refresh:
        print("\033[2m[Cache refresh requested: tax brackets will be re-extracted for the selected country]\033[0m\n")

    mode = select_research_mode()

    if mode == "AI deep search (needs country, more accurate)":
        job_name = ask_text("Enter the name of the job you want to analyse")
        country = ask_text("Enter the name of the country")

        if contains_unsafe_characters(job_name) or contains_unsafe_characters(country):
            invalid_characters_error()
            sys.exit(1)

        seniority = select_seniority()

        searching_salary_ai(job_name, country, seniority)

        try:
            with loading_bar():
                salary = search_salary_with_ai(job_name, country, seniority)
        except Exception as e:
            ai_error_message(e)
            sys.exit(1)
    else:
        job_name = ask_text("Enter the name of the job you want to analyse")
        if contains_unsafe_characters(job_name):
            invalid_characters_error()
            sys.exit(1)

        seniority = select_seniority()

        searching_salary_jobdatalake(job_name, seniority)

        try:
            with loading_bar():
                salary = search_salary_job(job_name, seniority)
        except Exception as e:
            print(f"[ERROR]: Salary lookup failed: {e}")
            sys.exit(1)

        if salary <= 0:
            print("[ERROR]: Could not retrieve salary from jobdatalake. Try AI mode instead.")
            sys.exit(1)

        country = ask_text("Enter the country for tax calculation")
        if contains_unsafe_characters(country):
            invalid_characters_error()
            sys.exit(1)

    gross_annual_salary(salary)

    income_tax_for_country(country)
    try:
        with loading_bar():
            result = calculate_tax(country, salary, forced_refresh=refresh)
        tax = result["tax"]
    except Exception as e:
        print(f"[ERROR]: Tax calculation failed: {e}. Assuming 0 tax.")
        tax = 0.0

    monthly_neat_salary = salary - tax
    monthly_calculated_neat_salary(monthly_neat_salary, tax)

    monthly_salary = monthly_neat_salary / 12
    print(f"Estimated monthly salary: {monthly_salary:.2f} with taxes\n")


    answer = ""
    while True:
        answer = agree_to_continue()
        if answer == "yes":
            print("\n")
            continue_program()
            print("\n")
            break
        elif answer == "no":
            logging_out()
            print(" ")
            sys.exit()
        else:
            invalid_input()

    spending_profile_header()

    rent = ask_expense("🏠", "Housing / rent / mortgage")
    survival = ask_expense("🍎", "Survival (food, utilities...)")
    free = ask_expense("🚗", "Transport / leisure")

    total_expenses = rent + survival + free
    savings_capacity = monthly_salary - total_expenses
    spending_summary(rent, survival, free, total_expenses, savings_capacity, monthly_salary)

    if savings_capacity <= 0:
        mothly_expenses_exceeded()
        sys.exit(0)

    choice = ""
    while True:
        choice = agree_to_continue()
        if choice == "yes":
            print(" ")
            continue_program()
            print(" ")
            break
        elif choice == "no":
            print(" ")
            logging_out()
            print(" ")
            sys.exit()
        else:
            invalid_input()

    plan_objectives(savings_capacity)


if __name__ == "__main__":
    main()
