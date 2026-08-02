import math
import os
import threading
import time

import questionary
from rich.console import Console
from rich.live import Live
from rich.panel import Panel
from rich.progress import BarColumn, Progress, SpinnerColumn, TextColumn, TimeElapsedColumn
from rich.table import Table
from questionary import Style
from rich.rule import Rule
console = Console()
# Define a stunning custom theme for all interactive prompts
finance_flow_theme = Style([
    ('qmark', 'fg:#ff00ff bold'),          # The prefix icon (e.g., Change '?' to magenta)
    ('question', 'fg:#ffffff bold'),       # The text of the question itself (White & Bold)
    ('answer', 'fg:#00ffff bold'),         # The color of the answer AFTER they press enter (Cyan)
    ('pointer', 'fg:#ff00ff bold'),        # The active indicator arrow inside menus (Magenta)
    ('highlighted', 'fg:#00ffff bold'),    # The currently hovered choice text
    ('selected', 'fg:#00ffff'),            # An already selected item
])
def display_welcome_box():
    console.print(
        Panel(
            "[bold cyan]Welcome to FinanceFlow v1.0[/bold cyan]\n"
            "[dim]Your automated financial tracking companion[/dim]",
            title = "[bold magenta]=== FinanceFlow === [/bold magenta]",
            expand = False
        )
    )

def select_research_mode():
    choice = questionary.select(
        "Select research method:",
        choices = [
            "AI deep search (needs country, more accurate)",
            "Jobdatalake quick search (global estimate, no AI quota needed)"
        ],
        qmark = ">",
        style = finance_flow_theme
    ).ask()
    return choice

def select_seniority():
    choice = questionary.select(
        "Select seniority:",
        choices = ["Senior", "Mid level", "Manager", "Entry", "Staff"],
        qmark = ">",
        style = finance_flow_theme
    ).ask()
    return choice

def searching_salary_ai(job_name,  country, seniority):
    console.print(f"\n[bold blue]🔄 [AI is searching for the salary of '{job_name} / {country} / {seniority}]")

def ai_error_message(error_message: str):
    console.print(f"\n[bold red]🤖❌ AI Salary Search Failed[/bold red]")
    console.print(f"[dim]Details: {error_message}[/dim]\n")

def invalid_characters_error():
    console.print("[bold red]❌ [ERROR]: Input contains invalid characters.[/bold red]")

def gross_annual_salary(salary):
    console.print(f"[bold green]: Gross annual salary recieved: {salary:.2f}\n [/bold green]")

def monthly_calculated_neat_salary(monthly_neat_salary, tax):
    console.print(f"[bold green] With taxes depending on the annual income the salary will be {monthly_neat_salary:.2f} with a tax of {tax:.2f} [/bold green]")

def agree_to_continue():
    choice = questionary.select( 
        "Do you want to proceed?",
        choices = ["yes", "no"],
        
        style = finance_flow_theme
    ).ask()
    return choice

def continue_program(duration: float = 3.0):
    dots = [".", "..", "...", ".."]
    start = time.monotonic()
    idx = 0
    with Live(
        Panel("[bold magenta] Continuing the program [/bold magenta]", expand=False),
        console=console,
        refresh_per_second=8,
    ) as live:
        while time.monotonic() - start < duration:
            live.update(
                Panel(f"[bold magenta] Continuing the program{dots[idx % 4]} [/bold magenta]", expand=False)
            )
            time.sleep(0.25)
            idx += 1
        live.update(
            Panel("[bold magenta] Continuing the program... [/bold magenta]", expand=False)
        )

def logging_out(duration: float = 3.0):
    dots = [".", "..", "...", ".."]
    start = time.monotonic()
    idx = 0
    with Live(
        Panel("[bold magenta] Logging out [/bold magenta]", expand=False),
        console=console,
        refresh_per_second=8,
    ) as live:
        while time.monotonic() - start < duration:
            live.update(
                Panel(f"[bold magenta] Logging out{dots[idx % 4]} [/bold magenta]", expand=False)
            )
            time.sleep(0.25)
            idx += 1
        live.update(
            Panel("[bold magenta] Logging out... [/bold magenta]", expand=False)
        )

def mothly_expenses_exceeded():
    console.print("[bold orange][WARNING]: Your monthly expenses exceeded (or equal) your monthly salary, so there's no savings capacity left for financial objectives [/bold orange]")
    console.print(
        Panel("[bold magenta] Program ending... [/bold magenta]", expand=False)
    )

def savings(savings_capacity):
    console.print(f"[bold green] Savings capacity: {savings_capacity:.2f} [/bold green]")

def negative_input():
    console.print("[bold red]❌ [ERROR]: Input cannot be negative. Please try again [/bold red]")

def numerical_value():
    console.print("[bold red]❌ [ERROR]: Please enter a numerical value")

def searching_salary_jobdatalake(job_name, seniority):
    console.print(f"\n[bold blue] [Searching jobdatalake for '{job_name}' / {seniority}] [/bold blue]")

def income_tax_for_country(country):
    console.print(f"[bold blue] Calculating income tax for {country} [/bold blue]")

def invalid_input():
    console.print("[bold red]❌ [ERROR]: Invalid input [/bold red]")


expense_theme = Style([
    ('qmark', 'fg:magenta bold'),
    ('question', 'fg:magenta bold'),
    ('answer', 'fg:white bold'),
])


def spending_profile_header():
    console.rule("[bold cyan]Spending profile[/bold cyan]")
    console.print(
        Panel(
            "[bold cyan]Set up your monthly spending profile[/bold cyan]",
            expand=False,
        )
    )


def objectives_planning_header():
    console.rule("[bold cyan]Objectives planning[/bold cyan]")
    console.print(
        Panel(
            "[bold cyan]Plan your financial objectives[/bold cyan]",
            expand=False,
        )
    )


def clear_screen():
    os.system("cls" if os.name == "nt" else "clear")


def objective_header(number: int):
    console.print()
    console.print(
        Panel(f"[bold white]Objective #{number}[/bold white]", expand=False)
    )


def ask_objective_name() -> str:
    value = questionary.text(
        "🎯  Enter the name of the objective",
        qmark=">",
        style=expense_theme,
    ).ask()
    if value is None:
        return ""
    return value.strip()


def ask_objective_cost() -> float:
    while True:
        raw = questionary.text(
            "💰  Enter the total cost",
            qmark=">",
            style=expense_theme,
        ).ask()
        if raw is None:
            continue
        clean = raw.replace("$", "").replace(",", "").strip()
        try:
            value = float(clean)
        except ValueError:
            numerical_value()
            continue
        if value < 0:
            negative_input()
            continue
        return value


def ask_objective_percentage() -> float:
    while True:
        raw = questionary.text(
            "📊  What % of your monthly savings do you want to allocate here?",
            qmark=">",
            style=expense_theme,
        ).ask()
        if raw is None:
            continue
        clean = raw.replace("$", "").replace(",", "").strip()
        try:
            value = float(clean)
        except ValueError:
            numerical_value()
            continue
        if value < 0:
            negative_input()
            continue
        return value


def ask_objectives_count() -> int:
    while True:
        raw = questionary.text(
            "How many financial objectives do you want to plan?",
            style=finance_flow_theme,
        ).ask()
        if raw is None:
            continue
        clean = raw.replace("$", "").replace(",", "").strip()
        try:
            num = int(clean)
        except ValueError:
            numerical_value()
            continue
        if num <= 0:
            negative_input()
            continue
        return num


def ask_expense(icon: str, category: str) -> float:
    while True:
        raw = questionary.text(
            f"{icon}  {category}",
            qmark=">",
            style=expense_theme,
        ).ask()
        if raw is None:
            continue
        clean = raw.replace("$", "").replace(",", "").strip()
        try:
            value = float(clean)
        except ValueError:
            numerical_value()
            continue
        if value < 0:
            negative_input()
            continue
        return value


def allocation_note(total_percentage: float):
    console.print(
        f"[Note]: You allocated {total_percentage:.1f}% of your savings. "
        f"The remaining {100.0 - total_percentage:.1f}% will go into generic savings.",
        style="dim",
        markup=False,
    )


def allocation_report(names, costs, percentages, savings_capacity):
    table = Table(
        title="[bold blue]Allocation report[/bold blue]",
        header_style="bold magenta",
        expand=False,
    )
    table.add_column("Objective", style="cyan")
    table.add_column("Cost", justify="right")
    table.add_column("Allocation", justify="right", style="green")
    table.add_column("Monthly", justify="right")
    table.add_column("Estimated time", justify="right", style="green")
    for name, cost, pct in zip(names, costs, percentages):
        monthly = savings_capacity * (pct / 100.0)
        if monthly <= 0.0:
            table.add_row(name, f"{cost:.2f}", f"{pct}%", "—", "N/A")
            continue
        months = math.ceil(cost / monthly)
        table.add_row(name, f"{cost:.2f}", f"{pct}%", f"{monthly:.2f}", f"{months} months")
    console.print()
    console.print(table)
    console.rule()


def spending_summary(rent, survival, free, total_expenses, savings_capacity, monthly_salary):
    table = Table(title="[bold magenta]Analysis result[/bold magenta]", expand=False)
    table.add_column("Expense", style="cyan")
    table.add_column("Monthly", justify="right")
    table.add_row("🏠 Housing / rent / mortgage", f"{rent:.2f}")
    table.add_row("🍎 Survival", f"{survival:.2f}")
    table.add_row("🚗 Transport / leisure", f"{free:.2f}")
    table.add_row("", "")
    table.add_row("[bold]Total expenses[/bold]", f"[bold]{total_expenses:.2f}[/bold]")
    table.add_row("[bold]Net monthly salary[/bold]", f"[bold]{monthly_salary:.2f}[/bold]")
    color = "green" if savings_capacity >= 0 else "red"
    table.add_row(
        "[bold]Savings capacity[/bold]",
        f"[bold {color}]{savings_capacity:.2f}[/bold {color}]",
    )
    console.print()
    console.print(table)
    console.rule()


class loading_bar:
    def __init__(self, max_seconds: float = 20.0):
        self.max_seconds = max_seconds
        self._stop = threading.Event()
        self._progress = Progress(
            SpinnerColumn(),
            BarColumn(
                bar_width=40,
                style="white",
                complete_style="bold cyan",
                finished_style="bold magenta",
            ),
            TextColumn("[bold cyan]{task.percentage:>3.0f}%[/bold cyan]"),
            TimeElapsedColumn(),
            console=console,
        )
        self._thread = None
        self._task_id = None

    def __enter__(self):
        console.print()
        self._progress.start()
        self._task_id = self._progress.add_task("", total=100.0)
        self._thread = threading.Thread(target=self._animate, daemon=True)
        self._thread.start()
        return self

    def _animate(self):
        start = time.monotonic()
        while not self._stop.is_set():
            elapsed = time.monotonic() - start
            pct = min(90.0, (elapsed / self.max_seconds) * 100.0)
            self._progress.update(self._task_id, completed=pct)
            time.sleep(0.1)
        self._progress.update(self._task_id, completed=100.0)

    def __exit__(self, exc_type, exc_val, exc_tb):
        self._stop.set()
        if self._thread is not None:
            self._thread.join(timeout=0.5)
        self._progress.stop()
        console.print()
        return False


def ask_text(prompt: str) -> str:
    value = questionary.text(prompt, qmark=">", style=finance_flow_theme).ask()
    if value is None:
        value = ""
    return value.strip()

