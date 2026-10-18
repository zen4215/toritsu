type ButtonProps = {
    label: string;
    onPress: () => void;
}

export function Button({label, onPress}: ButtonProps) {
    return (
        <>
            <button type="button" onPointerDown={onPress} className="user-button" id={label}>
                {label}
            </button>
        </>
    );
}
