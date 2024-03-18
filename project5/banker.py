'''
********************************************************************
//
// Mohammed Sharfuddin Shawon
// 3/18/2024
// CSC 360  : Operating Systems
// Project#5: Implementation of Banker's Algorithm
// Instructor: Dr. Siming Liu
//
********************************************************************
'''

# input_file_name = "tests/test3.txt"
input_file_name = "s1.txt"

def read_input(file_name):
    """
    Read input from the specified file.

    Parameters:
    -----------
    file_name : str
        The name of the input file containing allocation, max, available, and request vectors.

    Returns:
    --------
    tuple
        A tuple containing the number of processes, number of resource types,
        allocation matrix, max matrix, available vector, and request vector.
    """
    with open(file_name, 'r') as file:
        lines = [line.strip() for line in file.readlines() if line.strip()]  # Skip empty lines
        processes = int(lines[0])
        resource_types = int(lines[1])

        allocation = [list(map(int, line.split())) for line in lines[2:2+processes]]
        max_matrix = [list(map(int, line.split())) for line in lines[2+processes:2+2*processes]]
        available = list(map(int, lines[2+2*processes].split()))

        # Parse the request vector line
        request_line = lines[3+2*processes].split(":")
        process_num = int(request_line[0])
        request = list(map(int, request_line[1].split()))

    return processes, resource_types, allocation, max_matrix, available, request

def print_matrix(matrix, label):
    """
    Print a matrix with labels.

    Parameters:
    -----------
    matrix : list of lists
        The matrix to be printed.
    label : list of str
        The labels for the matrix.
    """
    print("   ", end="")
    for l in label:
        print(l.rjust(3), end=" ")
    print()
    for i, row in enumerate(matrix):
        print(f"{i}: ", end="")
        for val in row:
            print(str(val).rjust(3), end=" ")
        print()

def compute_need(allocation, maximum):
    """
    Compute the need matrix.

    Parameters:
    -----------
    allocation : list of lists
        The allocation matrix.
    maximum : list of lists
        The maximum matrix.

    Returns:
    --------
    list of lists
        The computed need matrix.
    """
    need = []
    for i in range(len(allocation)):
        need.append([maximum[i][j] - allocation[i][j] for j in range(len(allocation[0]))])
    return need

def is_safe_state(processes, allocation, max_matrix, available):
    """
    Check if the system is in a safe state.

    Parameters:
    -----------
    processes : int
        Number of processes in the system.
    allocation : list of lists
        The allocation matrix.
    max_matrix : list of lists
        The max matrix.
    available : list
        The available vector.

    Returns:
    --------
    bool
        True if the system is in a safe state, False otherwise.
    """
    work = available.copy()
    finish = [False] * processes
    need = compute_need(allocation, max_matrix)

    while True:
        found = False
        for i in range(processes):
            if not finish[i] and all(need[i][j] <= work[j] for j in range(len(available))):
                found = True
                break

        if not found:
            break

        finish[i] = True
        for j in range(len(available)):
            work[j] += allocation[i][j]

    return all(finish)

def can_request_be_granted(process_num, request, allocation, available):
    """
    Check if a request can be granted.

    Parameters:
    -----------
    process_num : int
        The number of the process making the request.
    request : list
        The request vector.
    allocation : list of lists
        The allocation matrix.
    available : list
        The available vector.

    Returns:
    --------
    bool
        True if the request can be granted, False otherwise.
    """
    for i in range(len(available)):
        if request[i] > available[i]:
            return False

    new_allocation = allocation.copy()
    new_available = available.copy()
    for i in range(len(available)):
        new_allocation[process_num][i] += request[i]
        new_available[i] -= request[i]

    if is_safe_state(len(allocation), new_allocation, new_allocation, new_available):
        return True
    else:
        return False

def grant_request(process_num, request, allocation, available):
    """
    Grant the request and update the allocation and available vectors.

    Parameters:
    -----------
    process_num : int
        The number of the process making the request.
    request : list
        The request vector.
    allocation : list of lists
        The allocation matrix.
    available : list
        The available vector.

    Returns:
    --------
    list
        The updated allocation vector.
    list
        The updated available vector.
    """
    for i in range(len(available)):
        allocation[process_num][i] += request[i]
        available[i] -= request[i]

    return allocation, available

def main():
    # Read input from the file
    processes, resource_types, allocation, max_matrix, available, request = read_input(input_file_name)

    # Generate labels for columns dynamically
    column_labels = [chr(ord('A') + i) for i in range(resource_types)]

    # Print the number of processes and resource types
    print("There are {} processes in the system.".format(processes))
    print("There are {} resource types.".format(resource_types))

    # Print the Allocation Matrix
    print("\nThe Allocation Matrix is...")
    print_matrix(allocation, column_labels)

    # Print the Max Matrix
    print("\nThe Max Matrix is...")
    print_matrix(max_matrix, column_labels)

    # Compute and print the Need Matrix
    print("\nThe Need Matrix is...")
    need_matrix = compute_need(allocation, max_matrix)
    print_matrix(need_matrix, column_labels)

    # Echo and print the Available Vector
    print("\nThe Available Vector is...")
    print_matrix([available], column_labels)

    # Check if the system is in a safe state
    if is_safe_state(processes, allocation, max_matrix, available):
        print("\nTHE SYSTEM IS IN A SAFE STATE!\n")
    else:
        print("\nTHE SYSTEM IS NOT IN A SAFE STATE!\n")

    # Echo and print the Request Vector
    print("The Request Vector is...")
    print_matrix([request], column_labels)

    # Check if the request can be granted
    if can_request_be_granted(processes - 1, request, allocation, available):  # Subtract 1 from process number
        print("\nTHE REQUEST CAN BE GRANTED!\n")
        # Grant the request and update vectors
        allocation, available = grant_request(processes - 1, request, allocation, available)  # Subtract 1 from process number
        # Echo and print the updated Available Vector
        print("\nThe Updated Available Vector is...")
        print_matrix([available], column_labels)
    else:
        print("\nTHE REQUEST CANNOT BE GRANTED!\n")

if __name__ == "__main__":
    main()
